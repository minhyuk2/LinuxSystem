#define OPENSSL_API_COMPAT 0x10100000L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#define true 1
#define false 0

#define NAMEMAX 255
#define PATHMAX 4096
#define STRMAX 4096

#define HASH_MD5 33

int hash=33;

//backupnode를 만들 구조체이다.
typedef struct backupNode {
  char backupPath[PATHMAX];
  char newPath[PATHMAX];
  char originPath[PATHMAX];
  char *directoryName;
  struct stat statbuf;
  int countNum;
  struct backupNode *next;

} backupNode;

//backupNode의 헤드를 저장하여 전역변수로 사용하기 위한 구조체입니다.
 typedef struct fileList {
	 struct backupNode *head;
 }fileList;

fileList *forFileList;
fileList *forRemoveList;
backupNode *backupList;

//fileNode를 위한 구조체입니다.
typedef struct fileNode {
  char path[PATHMAX];
  backupNode *head;

  struct fileNode *next;
} fileNode;

//dirNode를 위한 구조체입니다.
typedef struct dirNode {
  char path[PATHMAX];
  char backupPath[PATHMAX];

  fileNode *filehead;
  struct dirNode *next;
} dirNode;

//dirNode의 헤드를 저장하여 전역변수로 사용하기 위한 구조체입니다.
typedef struct dirList {
  struct dirNode *head;
} dirList;

dirList *mainDirList;


typedef struct pathList {
  struct pathList_ *next;
  struct pathList_ *prev;
  char path[NAMEMAX];

} pathList;

typedef struct findedPathList {

	char originPath[PATHMAX];
	int countNum;
}findedPathList;

//queue를 만들기 위한 구조체다.
typedef struct queue{
     dirNode *first;
	 dirNode *last;
 }queue;


char exeNAME[PATHMAX];
char exePATH[PATHMAX];
char homePATH[PATHMAX];
char backupPATH[PATHMAX];

//ssu_help.c 파일에 들어있는 함수원형들 입니다.
void printBackup();
void printUsage();
void printRemove();
void printRecover();
void printList();
void printHelp();

//현재 시각을 12자리로 리턴하는 함수
char *getDate() {
	char *date = (char *)malloc(sizeof(char) * 14);
	time_t timer;
	struct tm *t;

	timer = time(NULL);
	t = localtime(&timer);

  sprintf(date, "%02d%02d%02d%02d%02d%02d",t->tm_year %100, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

  return date;
}

//해싱해서 파일이 같은지 비교하기 위해 사용하는 함수
int md5(char *target_path, char *hash_result)
{
	FILE *fp;
	unsigned char hash[MD5_DIGEST_LENGTH];
	unsigned char buffer[SHRT_MAX];
	int bytes = 0;
	MD5_CTX md5;

	if ((fp = fopen(target_path, "rb")) == NULL){
		printf("ERROR: fopen error for %s\n", target_path);
		return 1;
	}

	MD5_Init(&md5);

	while ((bytes = fread(buffer, 1, SHRT_MAX, fp)) != 0)
		MD5_Update(&md5, buffer, bytes);
	
	MD5_Final(hash, &md5);

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		sprintf(hash_result + (i * 2), "%02x", hash[i]);
	hash_result[HASH_MD5-1] = 0;

	fclose(fp);

	return 0;
}

//backupNode를 추가하는 함수이다.
backupNode* addBackupNode(backupNode *head,char originPath[], char backupPath[]) {
    backupNode* newNode = (backupNode*)malloc(sizeof(backupNode));
	backupNode* curr = (backupNode*)malloc(sizeof(backupNode));
	struct stat tmpbuf;
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(newNode->originPath, originPath);
    strcpy(newNode->backupPath, backupPath);
	//새로운 노드에 데이터를 저장합니다.	

	newNode->next = NULL;

	if(head==NULL){
		backupNode* newHead = (backupNode*)malloc(sizeof(backupNode));
		newHead = head;
		newHead->next = newNode;
		return newHead;
		
	}
	else{
		curr = head;
		while(curr->next !=NULL){
			curr = curr->next;
		}
		curr->next = newNode;
	}
    return head;
}


//원하는 노드를 찾아서 삭제하는 함수입니다.
backupNode* deleteBackupNode(backupNode *head, char *deletePath){
		
		backupNode *tempNode;
		tempNode = head;
		backupNode *deleteNode;
		if((tempNode->next->next ==NULL)&&(strcmp(deletePath,tempNode->next->backupPath)==0)){
			//한 개만 남았을 때에 대한 예외를 처리해줘야한다.
			head->next =NULL;
			return head;
		}

		while(tempNode->next != NULL){
		 if(strcmp(deletePath,tempNode->next->backupPath)==0){
				
				deleteNode=tempNode->next;
				tempNode->next= tempNode->next->next;
				deleteNode->next =NULL;
				free(deleteNode);
				return head;
		}//삭제를 할 노드를 찾아서 삭제해준다.
		 tempNode=tempNode->next;
		}
		return head;
}


//주어진 경로를 originPath로 갖고 있는 노드들의 링크드리시트를 만들어서 리턴한다.
backupNode*  searchNode(backupNode *head, char searchPath[]){
		backupNode *tempNode;
		tempNode = head;
		int count =0;
		backupNode *newHead = (backupNode*)malloc(sizeof(backupNode));
		while(tempNode ->next !=NULL){
			tempNode = tempNode -> next;
			if(strcmp(tempNode -> originPath,searchPath)==0){
				newHead = addBackupNode(newHead, tempNode->originPath,tempNode->backupPath);
			}
		}

		return newHead;
}
//directory주소를 통해 삭제된 파일들을 찾기 위한 함수이다.

backupNode*  searchDir(backupNode *head, char searchPath[]){
		backupNode *tempNode;
		tempNode = head;
		int count =0;
		backupNode *newHead = (backupNode*)malloc(sizeof(backupNode));
		while(tempNode ->next !=NULL){
			tempNode = tempNode -> next;
			if(strstr(tempNode -> originPath,searchPath)!=NULL){
				newHead = addBackupNode(newHead, tempNode->originPath,tempNode->backupPath);
			}
		}
		return newHead;
}

char* getFileName(char* filepath){
	  int idx;
	  char* filename;
	  for(idx = strlen(filepath)-1; filepath[idx] != '/'; idx--);
	  strcpy(filename, filepath+idx+1);
	  return filename;
} //경로의 끝에 항상 filename이 나오기에 이를 찾는 함수 생성


//directory를 삭제하는 함수다.
void removeEmptyDir(char *dirPath){
	struct dirent **namelist;
	int cnt;
	if((cnt = scandir(dirPath,&namelist,NULL,alphasort))<0){
		fprintf(stderr,"scandir에서 에러발생\n");
		exit(1);
	}
	//scandir리턴 값이 2개뿐이면 현재랑 상위폴더밖에 없기에 삭제할 수 있다.

	if(cnt == 2){
		    if (rmdir(dirPath) != 0) {
            printf("ERROR: can't remove directory.\n");
			}
	}
}

//queue를 연결하는 함수이다.
void enqueue(queue* Queue, char* dirPath){
	dirNode* newNode = (dirNode *)malloc(sizeof(dirNode));
	strcpy(newNode->path,dirPath);
	newNode->next=NULL;
	if(Queue->first == NULL){
		Queue->first = newNode;
		Queue->last = newNode;
		return;
	}
	else{
		dirNode *tempNode;
		tempNode = Queue->last->next;
		Queue ->last->next = newNode;
		newNode -> next = tempNode;
		Queue -> last = newNode;
		return;
	}
}

//큐에서 맨앞에 들어온 node를 빼는 함수이다.
dirNode*  dequeue(queue* Queue){
	dirNode *tempNode;
	tempNode = Queue->first;
	Queue->first = Queue->first->next;

	return tempNode;
} 


//fileNode를 추가하는 함수입니다.
fileNode *addFileNode(fileNode *head,char *path){
	fileNode* newNode = (fileNode*)malloc(sizeof(fileNode));
	strcpy(newNode->path,path);
	newNode ->next = NULL;
	if(head==NULL){
		head=newNode;
	}
	else{
		fileNode* tempNode;
		tempNode = head;
		while(tempNode->next!=NULL){
			tempNode = tempNode->next;
		}
		tempNode ->next = newNode;
	}
	return head;
}

//directory노드를 추가하는 함수입니다.
dirNode *addDirNode(dirNode* head, fileNode* headFile,char* path){
	dirNode* newNode = (dirNode*)malloc(sizeof(dirNode));
	newNode ->filehead = headFile;
	newNode ->next = NULL;
	strcpy(newNode->path,path);
	if(head == NULL){
		head = newNode;
		return head;
	}else{
		dirNode* tempNode;
		tempNode = head;
		while(tempNode -> next !=NULL){
			tempNode = tempNode->next;
		}
		tempNode->next = newNode;
		return head;
	}
}

//BFS를 사용하기 위해서 재귀적으로 반복되는 함수이다.
void backupRecursive(queue* Queue,char *path,int backupOption){//backup뿐만 아니라 다른 명령어의 -r옵션에 대하여 사용가능합니다.
	struct stat statbuf,tempbuf;
	struct dirent **namelist;
	char* tempPath = (char *)malloc(sizeof(char *)*PATHMAX);
	fileNode *headFile = (fileNode*)malloc(sizeof(fileNode));	
	fileNode *forCheckFile;
	dirNode *nextDir;
	backupNode *forDirBack = (backupNode *)malloc(sizeof(backupNode));
	int cnt;
	if(lstat(path,&statbuf)<0){
		fprintf(stderr,"ERROR: lstat cuase error\n");
		exit(1);
	}
	
	if((cnt = scandir(path,&namelist,NULL,alphasort)) == -1){
		fprintf(stderr,"ERROR: cannot scan dir.\n");
		exit(1);
	}
	for(int i = 0; i <cnt;i++){
		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
		strcpy(tempPath,path);
		strcat(tempPath,"/");
		strcat(tempPath,namelist[i]->d_name);
	 if(lstat(tempPath,&tempbuf)<0){
		 fprintf(stderr,"lstat cause error.\n");
		 exit(1);
	 }
	 if(S_ISDIR(tempbuf.st_mode)){
		enqueue(Queue,tempPath);
	 } //디렉토리인 경우에는 queue에 추가해줍니다.
	 if(S_ISREG(tempbuf.st_mode)){
		headFile = addFileNode(headFile,tempPath); 
	 }	//파일인 경우에는 파일 링크드리스트에 추가해줍니다.

	}
	int count =0;
	if(backupOption!=0){
		forCheckFile = headFile;
		forDirBack = searchDir(forFileList->head,path);
		while(forDirBack->next!=NULL){
			forDirBack= forDirBack->next;
			while(forCheckFile->next!=NULL){
				forCheckFile=forCheckFile->next;
				if(strcmp(forCheckFile->path,forDirBack->originPath)!=0&&(count =0)){
					headFile = addFileNode(headFile,forDirBack->originPath);
					count ++;
			}	
		}
		count = 0;

	}
	}
	mainDirList-> head = addDirNode(mainDirList->head,headFile,path);
	if(Queue->first ==NULL){
		return;
	}
	nextDir=dequeue(Queue);
	backupRecursive(Queue,nextDir->path,backupOption);

}

//directory에 접근가능한지 체크하여 접근가능하지 않으면 하위 디렉토리부터 생성해주는 것이다.
char *accessDirCheck(char *newPath){
				int idx;	   
				char *pastPath = (char *)malloc(strlen(newPath) + 1);
                char *temperPath = (char *)malloc(strlen(newPath)+1);
                strcpy(temperPath,newPath);
                  for(idx = strlen(temperPath)-1; temperPath[idx] != '/'|idx==0; idx--);
                    strcpy(pastPath, temperPath);
					pastPath[idx] = '\0'; 
				if(access(pastPath,F_OK)){
					if(mkdir(pastPath,0777)<0){
						return accessDirCheck(pastPath);
					}
				}
}

