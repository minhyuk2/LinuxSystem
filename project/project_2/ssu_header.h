#define OPENSSL_API_COMPAT 0x10100000L

#ifndef HEADER_GUARD_PLZ
#define HEADER_GUARD_PLZ


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <openssl/md5.h>
#include <openssl/sha.h>


#define true 1
#define false 0

#define HASH_MD5  33
#define HASH_SHA1 41

#define NAMEMAX 255
#define PATHMAX 4096
#define STRMAX 4096
#define PATH_MAX 4096
#define BUF_MAX 4096
#define FILE_MAX 256


#define CMD_ADD			0b0000001 //1
#define CMD_REM			0b0000010 //2
#define CMD_STA		    0b0000100 //4
#define CMD_COM 		0b0000101 //5
#define CMD_REV			0b0000110 //6
#define CMD_LOG			0b0001000
#define CMD_HELP		0b0010000
#define CMD_EXIT		0b0100000
#define NOT_CMD			0b0000000

#define OPT_A		0b00001
#define OPT_C		0b00010
#define OPT_D		0b00100
#define OPT_N		0b01000
#define NOT_OPT 	0b00000

int hash = 33; 
int maxLevel =1;

char exeNAME[PATHMAX];
char exePATH[PATHMAX];
char homePATH[PATHMAX];
char repoPATH[PATHMAX];
char stagingPATH[PATHMAX];
char commitPATH[PATHMAX];



char *commanddata[10]={
  "add", //0
  "remove", //1
	"status",  //2
	"commit",  //3 
	"revert",  //4
	"log",  //5
  "help", //6
  "exit" //7
  };


//파일의 변경된 부분들을 계산하기 위한 구조체입니다.
typedef struct change_File {
  int insertLine;
  int deleteLine;
  int fileCount;
} changeFile;

//exec해줄 때 넘겨주는 인자에 대한 구조체입니다.
typedef struct command_parameter {
 	char *command;
	char *filename;
	char *tmpname;
 	char *argv[10];
} command_parameter;

//path를 구해줄 때 쓰기 위한 구조체이다.
typedef struct pathList_ {
  struct pathList_ *next;
  struct pathList_ *prev;
  char path[NAMEMAX];

} pathList;


//스테이지에 올라온 것들을 링크드리스트로 관리하기 위해 사용하는 구조체입니다.
typedef struct stagingList_ {
  struct stagingList_ *next;
  struct stagingList_ *head;
  int level;
  char dirPath[PATHMAX];
  char file_name[FILE_MAX];
  char path[PATHMAX];
}stagingList;

//디렉토리를 스테이징에서 관리하기 위한 구조체입니다.
typedef struct dirList_{
  struct dirList_ *next;
  struct dirList_ *head;
  //디렉토리 리스트의 헤드를 설정해주는 것이다.
  struct stagingList_ *stageHead;
  //stagingList_의 헤더를 저장해서 넣어줘야한다.
  int level;
  char parentDir[PATHMAX];
  //부모 디렉토리를 적어둬서 얘의 계층구조를 이용할 수 있도록 해준다.
  char dirPath[PATHMAX];
  char dirName[STRMAX];
  int file_cnt;
  //파일의 개수가 필요할 수 있기 때문에 저장해준다.
}dirList;

//queue를 만들기 위한 구조체다.
typedef struct queue{
   dirList *first;
	 dirList *last;
 }queue;

//전역변수로 선언하여 링크드리스트를 관리할 때 유용하게 만든다.
stagingList *useStage;
dirList *useDirList;
stagingList *removeFileList;
dirList *removeList;

//help.c에 적혀 있는 함수들의 함수원형들입니다.
void printAll();
void printAdd();
void printRemove();
void printStatus();
void printCommit();
void printRevert();
void printLog();
void printHelp();
void printExit();


//queue를 연결하는 함수이다.
void enqueue(queue* Queue, char* dirPath){
	dirList* newNode = (dirList *)malloc(sizeof(dirList));
	strcpy(newNode->dirPath,dirPath);
	newNode->next=NULL;
	if(Queue->first == NULL){
		Queue->first = newNode;
		Queue->last = newNode;
		return;
	}
	else{
		dirList *tempNode;
		tempNode = Queue->last->next;
		Queue ->last->next = newNode;
		newNode -> next = tempNode;
		Queue -> last = newNode;
		return;
	}
}

//큐에서 맨앞에 들어온 node를 빼는 함수이다.
dirList*  dequeue(queue* Queue){
	dirList *tempNode;
	tempNode = Queue->first;
	Queue->first = Queue->first->next;

	return tempNode;
} 

//파일의 이름을 뽑아내는 함수입니다.
char *GetFileName(char file_path[]){
  char *file_name;
  while(*file_path) {
    if(*file_path == '/' && (file_path +1) != NULL) {
    file_name = file_path+1;
    }
    file_path++;
  }
  return file_name;
}

//스테이지에 올라간  파일의 트리 구조상 level을 구하는 함수입니다.
void getLevelForStage(stagingList *stageNode){
  // /(슬래시)의 개수로 레벨을 판단합니다.
  char *stageName = stageNode->path;
  int level =1 ;
  for(int i = 0; i< strlen(stageName);i++){
    if(stageName[i] == '/'){
      level++;
    }
  }
  if(maxLevel<level){
    maxLevel = level;
  }
  stageNode->level = level;
}

//스테이지에 올라간 디렉토리의 트리 구조상 level을 구하는 함수입니다.
void getLevelForDir(dirList *dirNode){
  char *dirName = dirNode->dirPath;
  int level =1 ;
  for(int i = 0; i<strlen(dirName);i++){
    if(dirName[i] == '/'){
      level++;
    }
  }
  if(maxLevel<level){
    maxLevel = level;
  }
  dirNode->level = level;
  
}


//stagingList를 추가하는 함수이다.
stagingList* addStagingList(stagingList *head, char* newPath,char *dirPath) {
  stagingList* newNode = (stagingList*)malloc(sizeof(stagingList));
	stagingList* curr = (stagingList*)malloc(sizeof(stagingList));
	char *name = (char*)malloc(sizeof(char)*BUF_MAX);
  struct stat tmpbuf;
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
	//새로운 노드에 데이터를 저장합니다.	

	newNode->next = NULL;
  strcpy(newNode->path,newPath);
  strcpy(newNode->dirPath,dirPath);
  sprintf(name,"%s",newPath+strlen(dirPath));
  strcpy(newNode->file_name,name);
  getLevelForStage(newNode);
  //새로운 노드에 데이터들을 넣어줍니다.

	if(head==NULL&&head->next==NULL){
		stagingList* newHead = (stagingList*)malloc(sizeof(stagingList));
		newHead = head;
  
		newHead->next = newNode;
		return newHead;	
	}
	else{
		curr = head;
		while(curr->next != NULL){
			curr = curr->next;
		}
		curr->next = newNode;
	}
    return head;
}

//파일들로 이루어진 링크드리스틀 입력받아 노드를 삭제해주는 함수입니다.
stagingList *deleteStagingList(stagingList *head, char* deletePath){

  stagingList *currNode;
  stagingList *deleteNode;

  currNode = head;
  if(currNode ->next == NULL){
    return head;
  }
  if(currNode ->next ->next == NULL){
    //한 개 밖에 없는 경우에 대한 예외처리입니다.
    if(strcmp(currNode ->next -> path,deletePath)==0) {
      //그게 삭제할 노드인 경우
      head->next = NULL;
     
      return head;
    }
  }
  while(currNode->next != NULL){
		 if(strcmp(deletePath,currNode->next->path)==0){
				
				deleteNode=currNode->next;
				currNode->next= currNode->next->next;
				deleteNode->next =NULL;
				free(deleteNode);
				return head;
		}//삭제를 할 노드를 찾아서 삭제해준다.
		 currNode=currNode->next;
		}
		return head;
  //삭제할 노드가 next 노드인 상태
}

//원하는 파일을 찾는 함수입니다.
int findingStagingNode(stagingList *head, char* findingPath){
  stagingList *currNode;
  currNode = head;
  while(currNode -> next !=NULL){
    currNode = currNode->next;
    if(strcmp(currNode->path,findingPath)==0){
      return 1;
      //찾으려는게 있는 경우에 1을 리턴한다.
    }
  }
return 0;
}

//같은 디렉토리가 존재하는지 확인한다.
int findingDirList (dirList *head, char *findPath){
  dirList *currNode;
  currNode = head;
  while(currNode->next !=NULL){
    currNode = currNode->next;
    if(strcmp(currNode->dirPath,findPath)==0){
      return 1;
      //존재하면 1을 리턴한다.
    }
  }
  return 0;

}



//스테이징에 올릴 디렉토리에 대한 노드를 연결해서 추가해주는 함수입니다.
dirList *addDirListNode(dirList *head, char *newPath,stagingList *stageHead,int fileCount){
  dirList *currNode;
  currNode = head;
  dirList *newNode = (dirList *)malloc(sizeof(dirList));
  char *dirName = (char *)malloc(sizeof(char) *STRMAX);
  char *parentPath = (char *)malloc(sizeof(char)*PATHMAX);
  int idx;
  int idx2;

  strcpy(newNode->dirPath,newPath);
  getLevelForDir(newNode);

  for( idx2 = strlen(newPath); newPath[idx2] != '/'; idx2--);
  strncpy(dirName,newPath+idx2,strlen(newPath)-idx2);
  dirName[strlen(newPath)-idx2] = '\0';
  strcpy(newNode->dirName,dirName);    
  for( idx = strlen(newPath)-1; newPath[idx] != '/'; idx--);
  strncpy(parentPath,newPath,idx);
  parentPath[idx] = '\0';
  strcpy(newNode->parentDir,parentPath);
  newNode->stageHead = stageHead;
  newNode->next = NULL;
  //디렉토리 구조체에 필요한 정보들을 넣어줍니다.
    while(currNode->next !=NULL){
    currNode = currNode->next;
  } //뒤로 간다음에 노드를 추가해주면 된다.
  currNode ->next = newNode;
} //이러면 노드를 추가해서 넣어주는 것이다.



//스테이징에 올릴 디렉토리에 대한 노드를 재귀함수를 이용한 BFS로 구현하여 
void addDirList(queue* Queue,dirList *head, char* newPath, stagingList *stageHead){
  dirList *currNode;
  dirList *nextNode;

  dirList *newNode = (dirList *)malloc(sizeof(dirList));
  //디렉토리 리스트에 대한 동적할당을 해주는 것이다.

  currNode = head;
  struct stat statbuf;
  struct stat tempbuf; 
  struct dirent **namelist;
  int cnt;
  int fileCount=0;
  char *tempPath = (char *)malloc(sizeof(char)*PATHMAX);
  stagingList *newHead = (stagingList*)malloc(sizeof(stagingList));
  
  if(lstat(newPath,&statbuf)<0){
    fprintf(stderr,"lstat error\n");
    exit(1);
  }

if((cnt = scandir(newPath,&namelist,NULL, alphasort)) == -1){
		fprintf(stderr,"ERROR: cannot scan dir.\n");
		exit(1);
	}

	for(int i = 0; i <cnt;i++){
		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
		strcpy(tempPath,newPath);
		strcat(tempPath,"/");
		strcat(tempPath,namelist[i]->d_name);
	 if(lstat(tempPath,&tempbuf)<0){
		 fprintf(stderr,"lstat cause error.\n");
		 exit(1);
	 }
	 if(S_ISDIR(tempbuf.st_mode)){
    if(findingDirList(head,tempPath)==0){
		  enqueue(Queue,tempPath);
    }
	 } //디렉토리인 경우에는 queue에 추가해줍니다.
	 if(S_ISREG(tempbuf.st_mode)){
    if(findingStagingNode(stageHead,tempPath)==1){
      deleteStagingList(stageHead,tempPath);
    }//있으면 삭제하고 디렉토리 안의 리스트로 만들어주는 식으로 작동합니다.
    newHead = addStagingList(newHead,tempPath,newPath);
    fileCount++;
	 }	//파일인 경우에는 파일 링크드리스트에 추가해줍니다.
  }
  addDirListNode(head,newPath,newHead,fileCount);
  if(Queue->first ==NULL){
		return;
	}
  nextNode = dequeue(Queue);
  //재귀적으로 수행해줍니다.
  addDirList(Queue,head,nextNode->dirPath,stageHead);
}

//디렉토리 링크드리스트 안에서 해당하는 노드를 찾는 함수입니다.
int searchingNode(dirList* head, char * findPath, int option){
  //option이 1이면 file에 대한 것이고 option이 0인 경우 디렉토리에 대해 진행해준다.
  stagingList *currNode;
  dirList* currDir;
  currDir = head;
  if(option == 1){
    while(currDir->next != NULL){
      currDir = currDir->next;
      currNode = currDir->stageHead;

      while(currNode->next !=NULL){
        currNode = currNode -> next;
        if(strcmp(currNode->path,findPath)==0){
          return 1;
        }
      }
    }
  }
  if(option == 0){
    while(currDir->next != NULL){
      currDir = currDir -> next;
      if(strcmp(currDir->dirPath,findPath)==0){
        return 1;
        //찾으려는 값이 있는 경우에는 1을 리턴한다.
      }
    }
  }
  return 0;
}

//디렉토리 안에 있는 파일 링크드 리스트에서 해당하는 것을 찾아 삭제시켜주는 함수입니다.
void deleteNodeInDir(dirList *head, char *findPath){
  dirList *currNode;
  stagingList *currStageList;

  currNode = head;
  //이중 배열로 디렉토리 링크드리시트를 돌면서 해당하는 파일이 있는지 찾아줍니다.
  while(currNode->next !=NULL){
    currNode = currNode->next;
    currStageList = currNode->stageHead;
    while(currStageList->next!=NULL){
      currStageList = currStageList->next;
      if(strcmp(currStageList->path,findPath)==0){
        deleteStagingList(currNode->stageHead,findPath);
        return;
      }//dir밑의 링크드 리스트에서 삭제시켜 준다.
    }
  }
}

//디렉토리 링크드 리스트에서 디렉토리를 찾아 삭제해주는 함수입니다.
dirList *deleteDirNode(dirList *head, char*deletePath){
  //여기서 실질적으로 링크드 리스트에서 삭제하는 과정을 진행시켜준다.
  dirList *currNode;
  currNode = head;
  dirList *deleteNode;

  if(currNode ->next == NULL){
    return head;
  }
  if(currNode ->next ->next == NULL){
    //한 개 밖에 없는 경우
    if(strcmp(currNode ->next ->dirPath,deletePath)==0) {
      //그게 삭제할 노드인 경우
      head->next = NULL;
      return head;
    }
  }
  while(currNode->next != NULL){
		 if(strcmp(deletePath,currNode->next->dirPath)==0){
				deleteNode=currNode->next;
				currNode->next= deleteNode->next;
				deleteNode->next =NULL;
				free(deleteNode);
				return head;
		}//삭제를 할 노드를 찾아서 삭제해준다.
		 currNode=currNode->next;
		}
		return head;
}


//디렉토리의 트리 구조대로 삭제하기 위해서 상위디렉토리가 삭제되면 하위 디렉토리도 삭제되게끔 구현한 함수입니다.
void deleteDir(dirList *head, char *findPath,int option){
  //option이 0인 경우에는 이렇게 해주면 되는 것이다.
  //처음으로는 일단 찾으려는 path를 넣어주고 시작하는 것이다.
  dirList *currNode;
  currNode = head;

  
 //여기서는 이제 링크드 리스트를 돌려가면서 헤더를 parent같은 애들을 찾아내는 것이다.
  if(currNode ==NULL){
    printf("comehere\n");
    return;
  }

  if(option==0){
  while(currNode->next != NULL){
    currNode = currNode->next;
    if(strcmp(currNode->dirPath,findPath)==0){
      head = deleteDirNode(head,findPath);
      deleteDir(head,findPath,1);
      break;
    }//처음일 때는 자기 자신만 없애면 되니까 break;해주면 되는 것입니다.
    }
  }

  else if(option ==1){
  //이제는 재귀적으로 부모의 주소가 얘랑 같은 애들 찾아서 넣으면 되는 것이다.

  while(currNode->next !=NULL){
    currNode = currNode -> next;
    if(strcmp(currNode->parentDir,findPath)==0){
      head = deleteDirNode(head,currNode->dirPath);
      deleteDir(head,currNode->dirPath,1);
      currNode = head;
    }
  }
  }
}


//해시 값을 비교하기 위해서 md5를 사용하는 것입니다.
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


//특정 구분자를 문자열을 분리하는 함수입니다.
char *Tokenize(char *str, char *del) {
  int i = 0;
  int del_len = strlen(del);
  static char *tmp = NULL;
  char *tmp2 = NULL;

  if(str != NULL && tmp == NULL) {
    tmp = str;
  }

  if(str == NULL && tmp == NULL) {
    return NULL;
  }

  char *idx = tmp;

  while(i < del_len) {
    if(*idx == del[i]) {
      idx++;
      i = 0;
    } else {
      i++;
    }
  }
  if(*idx == '\0') {
    tmp = NULL;
    return tmp;
  }
  tmp = idx;

  while(*tmp != '\0') {
  
    for(i = 0; i < del_len; i++) {
      if(*tmp == del[i]) {
        *tmp = '\0';
        break;
      }
    }
    tmp++;
    if(i < del_len) {
      break;
    }
  }
  return idx;
}

//특정한 문자열 기준으로 토큰화하여 잘라주는 함수입니다.
char **GetSubstring(char *str, int *cnt, char *del) {
  *cnt = 0;
  int i = 0;
  char *token = NULL;
  char *templist[100] = {NULL, };
  char *repli = (char *)malloc(sizeof(char)*PATH_MAX);
  strcpy(repli,str);
  token = Tokenize(repli, del);
  if(token == NULL) {
    return NULL;
  }

  while(token != NULL) {
    templist[*cnt] = token;
    *cnt += 1;
    token = Tokenize(NULL, del);
  }
  
	char **temp = (char **)malloc(sizeof(char *) * (*cnt + 1));
	for (i = 0; i < *cnt; i++) {
		temp[i] = templist[i];
	}
	return temp;
}

//절대 경로로 변환시켜주는 함수입니다.
int ConvertPath(char* origin, char* resolved) {
  int idx = 0;
  int i;
  char *path = (char *)malloc(sizeof(char *) * PATHMAX);
  char *tmppath = (char *)malloc(sizeof(char *) * PATHMAX);
  char **pathlist;
  int pathcnt;

  if(origin == NULL) {
    return -1;
  }

  if(origin[0] == '~') {
    sprintf(path, "%s%s", homePATH, origin+1);
  } else if(origin[0] != '/') {
    sprintf(path, "%s/%s", exePATH, origin);
  } else {
    sprintf(path, "%s", origin);
  }

  if(!strcmp(path, "/")) {
    resolved = "/";
    return 0;
  }

  if((pathlist = GetSubstring(path, &pathcnt, "/")) == NULL) {
    return -1;
  }

  pathList *headpath = (pathList *)malloc(sizeof(pathList));
  pathList *currpath = headpath;

  for(i = 0; i < pathcnt; i++) {
    if(!strcmp(pathlist[i], ".")) {
      continue;
    } else if(!strcmp(pathlist[i], "..")) {
      currpath = currpath->prev;
      currpath->next = NULL;
      continue;
    }

    pathList *newpath = (pathList *)malloc(sizeof(pathList));
    strcpy(newpath->path, pathlist[i]);
    currpath->next = newpath;
    newpath->prev = currpath;

    currpath = currpath->next;
  }

  currpath = headpath->next;

  strcpy(tmppath, "/");
  while(currpath != NULL) {
    strcat(tmppath, currpath->path);
    if(currpath->next != NULL) {
      strcat(tmppath, "/");
    }
    currpath = currpath->next;
  }

  strcpy(resolved, tmppath);

  return 0;
}


//절대경로를 상대경로로 바꿔주는 함수입니다.
int realpath_2_relative(char *realpath, char *returnPath){
  
  char *relative = malloc(sizeof(char)*PATHMAX);
  char *returnChar = malloc(sizeof(char)*PATHMAX);

  //절대 경로인 경우다. 상대 경로일 때도 .으로 시작하는 지와 .으로 시작안하는 것도 구분해야한다.
  if(realpath[0]=='/'){

  relative = realpath + strlen(exePATH);
  sprintf(returnChar,".%s",relative);
  strcpy(returnPath,returnChar);
  }else if(strcmp(realpath,"./")==0){
    strcpy(returnPath,".");
  }else if(realpath[0]=='.'){
    //그냥 .으로 시작하는 상대경로가 주어진 경우
    strcpy(returnPath,realpath);
    
  }else{
    sprintf(relative, "./%s" ,realpath);
    strcpy(returnPath,relative);
    //상대경로일 경우에는 그냥 앞에 ./을 붙혀서 출력해준다.
    }
    return 0;
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

//링크드리스트를 level을 기준으로 정렬해주는 함수입니다.
void sortList(dirList **head) {
    dirList *sorted = NULL;
    dirList *current = *head;

    while (current != NULL) {
        dirList *next = current->next;
        // 정렬된 리스트에서 적절한 위치를 찾아 삽입
        if (sorted == NULL || sorted->level >= current->level) {
            current->next = sorted;
            sorted = current;
        } else {
            dirList *temp = sorted;
            while (temp->next != NULL && temp->next->level < current->level) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }

    *head = sorted; // 정렬된 리스트를 원래 리스트의 헤더에 할당
}

//스테이지에 올라간 함수와 디렉토리로 올라간 것들을 총합하여 하나의 디렉토리 리스트로 만들어주는 함수입니다.
void collectDir(dirList *dirHead, stagingList *stageHead){
  dirList * currDirHead = dirHead;
  stagingList *currStageHead = stageHead;
  int option = 0;


  while(currStageHead->next != NULL){
    currStageHead = currStageHead->next;

    currDirHead = useDirList->head;
    //디렉토리가 하나도 존재하지 않는 경우에 대한 예외처리가 필요합니다.
    if(currDirHead->next==NULL){
        int idx,idx2;
        dirList *newNode = (dirList *)malloc(sizeof(dirList));
        char *dirName = (char *)malloc(sizeof(char) *STRMAX);
        char *parentPath = (char *)malloc(sizeof(char)*PATHMAX);
        strcpy(newNode->dirPath,currStageHead->dirPath);
        getLevelForDir(newNode);
        for( idx2 = strlen(newNode->dirPath); newNode->dirPath[idx2] != '/'; idx2--);
        strncpy(dirName,newNode->dirPath+idx2,strlen(newNode->dirPath)-idx2);
        dirName[strlen(newNode->dirPath)-idx2] = '\0';
        strcpy(newNode->dirName,dirName);    
        for( idx = strlen(newNode->dirPath)-1; newNode->dirPath[idx] != '/'; idx--);
        strncpy(parentPath,newNode->dirPath,idx);
        parentPath[idx] = '\0';
        strcpy(newNode->parentDir,parentPath);
        stagingList *newStagingList = (stagingList*)malloc(sizeof(stagingList));
        newStagingList->next = NULL;
        //새로운 노드에 대한 설정을 해줍니다.
        newStagingList = addStagingList(newStagingList,currStageHead->path,currStageHead->dirPath);
        //새로운 노드를 처음으로 설정하여 새로운 디렉토리 리스트를 만듭니다.
        newNode->stageHead = newStagingList;
        currDirHead->next = newNode;
        newNode->next = NULL;
        continue;
    }
    //일반적인 경우에 대한 부분입니다.
    while(currDirHead->next != NULL){
        currDirHead = currDirHead->next;
        if(strcmp(currDirHead->dirPath,currStageHead->dirPath)==0){
          currDirHead->stageHead  = addStagingList(currDirHead->stageHead,currStageHead->path,currStageHead->dirPath);
          option = 1;
          break;
        }
    }
    //한 번 찾았는데 없는 경우에는 다시 찾게 됩니다.
    if(option != 1){
      currDirHead = useDirList->head;
      while(currDirHead->next != NULL){
       if(currDirHead->next->level >= currStageHead->level){
        dirList *tempNode;
        tempNode = currDirHead->next;
        dirList *newNode = (dirList *)malloc(sizeof(dirList));
        char *dirName = (char *)malloc(sizeof(char) *STRMAX);
        char *parentPath = (char *)malloc(sizeof(char)*PATHMAX);
        int idx;
        int idx2;
        strcpy(newNode->dirPath,currStageHead->dirPath);
        getLevelForDir(newNode);
        for( idx2 = strlen(newNode->dirPath); newNode->dirPath[idx2] != '/'; idx2--);
        strncpy(dirName,newNode->dirPath+idx2,strlen(newNode->dirPath)-idx2);
        dirName[strlen(newNode->dirPath)-idx2] = '\0';
        strcpy(newNode->dirName,dirName);    
      for( idx = strlen(newNode->dirPath)-1; newNode->dirPath[idx] != '/'; idx--);
        strncpy(parentPath,newNode->dirPath,idx);
        parentPath[idx] = '\0';
        strcpy(newNode->parentDir,parentPath);
        stagingList *newStagingList = (stagingList*)malloc(sizeof(stagingList));
        //동적할당해서 노드를 생성해준다.
        newStagingList->next = NULL;
        newStagingList = addStagingList(newStagingList,currStageHead->path,currStageHead->dirPath);
        newNode->stageHead = newStagingList;
        currDirHead->next = newNode;
        newNode->next = tempNode;
        break;
        //삭제 없이 이렇게 연결해주면 다 연결된 것입니다.
      }
      currDirHead = currDirHead->next;
    }
  }
  option = 0;
  }
  //마지막으로 처음부터 돌면서 전체 순서를 맞춰주면 됩니다.
  currDirHead = useDirList->head;
  sortList(&currDirHead);
}

// 디렉토리 안에 있는 모든 파일들을 BFS로 순회하여 stagingList로 만드는 함수입니다.
//option이 0일 경우 repo함수를 포함해서 하는 경우입니다.
void traverseDirectory(const char *dirPath, stagingList **head, char *unchangeName,int option) {
    struct dirent **namelist;
    int n;
    char *newName = (char *)malloc(sizeof(char)*PATH_MAX);

    // 디렉토리 내 파일들에 대한 정보를 얻어옵니다.
   if ((n = scandir(dirPath, &namelist, NULL, alphasort))<0){
    fprintf(stderr,"scandir error\n");
    return;
   }
    // 현재 디렉토리에 있는 파일들을 링크드 리스트에 추가
    for (int i = 0; i < n; ++i) {
        // "."와 ".." 디렉토리는 건너뜁니다.
        if (strcmp(namelist[i]->d_name, ".") == 0 || strcmp(namelist[i]->d_name, "..") == 0) {
            free(namelist[i]);
            continue;
        }
        if(option == 1){
          //option이 1인 경우에는 레포를 빼줍니다.
          if(strcmp(namelist[i]->d_name,".repo")==0){
            continue;
          }
        }
        // 파일 경로 생성해줍니다.
        char filePath[PATH_MAX];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, namelist[i]->d_name);
        // 파일 정보 구성해줍니다.
        if(namelist[i]->d_type == DT_REG){
        stagingList *newNode = (stagingList *)malloc(sizeof(stagingList));
        if (newNode != NULL) {
            newNode->next = NULL;
            newNode->head = NULL;
            strcpy(newNode->dirPath, dirPath);
            sprintf(newName,"%s",filePath+strlen(unchangeName)+1);
            newName[strlen(newName)]='\0';
            strcpy(newNode->file_name,newName);
            strcpy(newNode->path, filePath);
            getLevelForStage(newNode);
            // 링크드 리스트에 추가
            if (*head == NULL) {
                *head = newNode;
            } else {
                stagingList *temp = *head;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        }
        }
            // 디렉토리인 경우 재귀적으로 탐색해줍니다.
            if (namelist[i]->d_type == DT_DIR) {
                traverseDirectory(filePath, head,unchangeName,option);
            }
        // 메모리 해제해줍니다.
        free(namelist[i]);
    }
    // namelist의 메모리 해제해줍니다.
    free(namelist);
}


//두 파일의 삭제된 문장과 추가된 문장을 찾는 함수입니다.
void compareFile(char *oldPath, char *newPath,changeFile *changeCollect){
  FILE *old,*new;
  char *bufOld = (char *)malloc(sizeof(char)*BUF_MAX);
  char *bufNew = (char *)malloc(sizeof(char)*BUF_MAX);
  if((old = fopen(oldPath,"rb"))==NULL){
    fprintf(stderr,"fopen error for %s\n",oldPath);
    return;
  }
  if((new= fopen(newPath,"rb"))==NULL){
    fprintf(stderr,"fopen error for %s\n",newPath);
    return;
  }
  int option = 0;
  //같은 줄이 있는지 한 줄씩 비교하는 것입니다.
  while(fgets(bufOld,BUF_MAX,old)!=NULL){
    while(fgets(bufNew,BUF_MAX,new)!=NULL){
      if((strcmp(bufNew,bufOld)==0)){
        option = 1;
      }
    }
    if(option ==0){
      changeCollect->deleteLine++;
    }
    option = 0 ;
    fseek(new,0,SEEK_SET);
  }
  option = 0;
  fseek(new,0,SEEK_SET);
  fseek(old,0,SEEK_SET);
  //반대로 생성된 줄이 있는지 한 줄씩 비교해서 확인하는 것입니다.
  while(fgets(bufNew,BUF_MAX,new)!=NULL){
    while(fgets(bufOld,BUF_MAX,old)!=NULL){
      if((strcmp(bufNew,bufOld)==0)){
        option = 1;
      }
    }
    if(option ==0){
      changeCollect->insertLine++;
    }
    option = 0 ;
    fseek(old,0,SEEK_SET);
  }
  fseek(new,0,SEEK_SET);
  fseek(old,0,SEEK_SET);
  int clearLineCountNew =0;
  int clearLineCountOld =0;

  //비어있는 문자열에 대한 개수를 따로 예외적으로 처리해줍니다.
  while(fgets(bufNew,BUF_MAX,new)!=NULL){
    if(bufNew[0]=='\n'){
          clearLineCountNew++;
        }
  }
  while(fgets(bufOld,BUF_MAX,old)!=NULL){
    if(bufOld[0] == '\n'){
          clearLineCountOld++;
      }
  }
  if(clearLineCountNew > clearLineCountOld){
    changeCollect->insertLine = changeCollect->insertLine + (clearLineCountNew-clearLineCountOld);
  }
  else if(clearLineCountOld > clearLineCountNew){
    changeCollect->deleteLine = changeCollect->deleteLine + (clearLineCountOld-clearLineCountNew);
  }
  fclose(old);
  fclose(new);
}

void countFileLine(changeFile *changeFile,char* filePath){
  FILE* fp;
  char *buf = (char *)malloc(sizeof(char)*BUF_MAX);
  if((fp= fopen(filePath,"rb"))==NULL){
    fprintf(stderr,"fopen error\n");
    return;
  }

  while(fgets(buf,BUF_MAX,fp)!=NULL){
      changeFile->insertLine++;
  }
  fclose(fp);
}


#endif
