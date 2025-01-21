#include "ssu_header.h"

int RemoveFile(char *path){
	int num; //scanf할 때 사용하기 위함임
	struct stat statbuffer;
	struct stat tempbuffer;
	char *originPath = (char *)malloc(sizeof(char*) * PATHMAX);
	backupNode* head = (backupNode*)malloc(sizeof(backupNode));
	int fd;
	char *dateData = (char *)malloc(sizeof(char *)*14);
	int count = 1;
	char *emptyChar = (char *)malloc(sizeof(char *)*STRMAX);
	char *tempChar= (char *)malloc(sizeof(char*)*STRMAX);
	char *forPrintChar=(char *)malloc(sizeof(char *)*STRMAX);
	char *removeDirPath = (char *)malloc(sizeof(char *)*PATHMAX);
	backupNode* tempNode=(backupNode*)malloc(sizeof(backupNode));
	forRemoveList = (fileList *)malloc(sizeof(fileList));
	char *backupLog = (char *)malloc(sizeof(char *)* PATHMAX *2 +10);
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}//strlen로 경로가 길 경우 에러처리를 해준다.
	if(realpath(path,originPath)==NULL){
		if(path[0] =='.'){
			path++;
			sprintf(originPath,"%s%s",exePATH,path);
		}
		else{
			strcpy(originPath,path);
		}
	}//파일이 없을 경우에 realPath가 안될 때를 대비하여 만들어 둔 것이다.
	head = searchNode(forFileList->head,originPath);
	if(head ->next ==NULL){
	if(lstat(originPath,&tempbuffer)<0){
		fprintf(stderr,"ERROR: this is not existing path.\n");
		exit(1);
	}
	if(S_ISDIR(tempbuffer.st_mode)){
		printf("ERROR: this is directory path\n");
		exit(1);
	}
	}
	//head->next 가 없는 경우에는 해당 파일이 존재하지 않을 수 있기에 디렉토리인지 없는 경로인지 판단한다
	head = searchNode(forFileList->head,originPath);
	forRemoveList -> head = head;
	tempNode = head;

	if(tempNode->next==NULL){
		printf("ERROR: There is no file for remove.\n");
		return 1;
	} //삭제할 파일이 없는 경우
	if(tempNode ->next->next==NULL){
	//삭제할 파일이 1개만 있는 경우
		tempNode = tempNode ->next;
		if(remove(tempNode->backupPath)	!=0){
			printf("ERROR: can't remove file.\n");
		}
		strcpy(removeDirPath,tempNode->backupPath);
		//dir이 비어있는지 검사하고 삭제하기 위해서 사용함.
		tempChar=strstr(removeDirPath,"backup");
		strncpy(forPrintChar,tempChar + 7, 12);
		sprintf(emptyChar,"%s/%s",backupPATH,forPrintChar);
		removeEmptyDir(emptyChar);
		//directory가 비어있는 경우에 삭제를 진행한다.
		dateData = getDate();
		if((fd = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
			fprintf(stderr, "open file cause error3\n");
			exit(1);
		}
		//log함수를 열어준더ㅏ.
		sprintf(backupLog, "%s : \"%s\" removed by \"%s\"\n",dateData,tempNode ->backupPath, tempNode -> originPath);
		write(fd,backupLog,strlen(backupLog));
		printf("\"%s\" removed by \"%s\"\n",tempNode->backupPath,tempNode->originPath);
		return 1;
	}
	printf("backup files of %s\n",originPath);
	printf("0. exit\n");
	while(tempNode->next !=NULL){
			printf("%d. ",count++);
			//하나씩 출려되게끔 하면 된다.
			tempChar=strstr(tempNode->next->backupPath,"backup");
			strncpy(forPrintChar, tempChar + 7, 12);
			if(lstat(tempNode->next->backupPath,&statbuffer)<0){
				fprintf(stderr,"lstat error\n");
				exit(1);
			}
			
			//이 부분을 통해서 12자리의 날짜를 뽑아낼 수 있음
			printf("%s            %ldbytes\n",forPrintChar,statbuffer.st_size);
			tempNode = tempNode->next;
		}	
	printf(">>");
	scanf("%d",&num);
	if(num ==0)
			return 1;
	else if(num > count){
			printf("ERROR: this is wrong number.\n");
			return 1;
		}
	tempNode = head;
	for(int i =0; i<num;i++){
			tempNode = tempNode ->next;
		}//해당하는 숫자의 파일로 node로 이동해서 삭제를 진행한다.
		if(remove(tempNode->backupPath)	!=0){
			printf("ERROR: file can't remove\n");
		}
		strcpy(removeDirPath,tempNode->backupPath);
		//dir이 비어있는지 검사하고 삭제하기 위해서 사용함.
		tempChar=strstr(removeDirPath,"backup");
		strncpy(forPrintChar,tempChar + 7, 12);
		sprintf(emptyChar,"%s/%s",backupPATH,forPrintChar);
		removeEmptyDir(emptyChar);	
		//없으면 디렉토리를 삭제
		dateData = getDate();
		if((fd = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
			fprintf(stderr, "open file cause error3\n");
			exit(1);
		}
		//로그 함수를 열어서 작성한다.
		sprintf(backupLog, "%s : \"%s\" removed by  \"%s\"\n",dateData,tempNode->backupPath, tempNode->originPath);
		write(fd,backupLog,strlen(backupLog));
		printf("\"%s\" removed by \"%s\"\n",tempNode->backupPath,tempNode->originPath);
	//이제 이 링크드리스트를 이용하여 출력시키고 만약에 찾은 것에 대해서는 노드 삭제	
}

//remove에 -a옵션이 들어왔을 때 처리하기 위한 함수이다.
int RemoveForA(char *path ){
	struct stat statbuffer;
	struct stat tempbuffer;
	char *originPath = (char *)malloc(sizeof(char*) * PATHMAX);
	backupNode* head = (backupNode*)malloc(sizeof(backupNode));
	int fd;
	char *dateData = (char *)malloc(sizeof(char *)*14);
	char *emptyChar = (char *)malloc(sizeof(char *)*STRMAX);
	char *tempChar= (char *)malloc(sizeof(char*)*STRMAX);
	char *forPrintChar=(char *)malloc(sizeof(char *)*STRMAX);
	char *removeDirPath = (char *)malloc(sizeof(char *)*PATHMAX);
	backupNode* tempNode=(backupNode*)malloc(sizeof(backupNode));
	forRemoveList = (fileList *)malloc(sizeof(fileList));
	char *backupLog = (char *)malloc(sizeof(char *)* PATHMAX *2 +10);
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path,originPath)==NULL){
		if(path[0] =='.'){
			path++;
			sprintf(originPath,"%s%s",exePATH,path);
		}
		else{
			strcpy(originPath,path);
		}
	}
	//realpath가 안될 경우에 경로를 만들어준다.
	head = searchNode(forFileList->head,originPath);
	if(head->next ==NULL){
	if(lstat(originPath,&tempbuffer)<0){
		fprintf(stderr,"ERROR: this is not existing path.\n");
		exit(1);
	}
	if(S_ISDIR(tempbuffer.st_mode)){
		printf("ERROR: this is directory\n");
		exit(1);
	}
	}
	head = searchNode(forFileList->head,originPath);
	forRemoveList -> head = head;
	tempNode = head;
	if((fd = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
		fprintf(stderr, "open file cause error3\n");
		exit(1);
         }
	dateData = getDate();
	while(tempNode ->next !=NULL){
	tempNode = tempNode ->next;
		if(remove(tempNode->backupPath)	!=0){
			printf("파일을 삭제하는데 실패함\n");
		}
		strcpy(removeDirPath,tempNode->backupPath);
		//dir이 비어있는지 검사하고 삭제하기 위해서 사용함.
		tempChar=strstr(removeDirPath,"backup");
		strncpy(forPrintChar,tempChar + 7, 12);
		sprintf(emptyChar,"%s/%s",backupPATH,forPrintChar);
		removeEmptyDir(emptyChar);
		sprintf(backupLog, "%s : \"%s\" removed by \"%s\"\n",dateData,tempNode ->backupPath, tempNode -> originPath);
		write(fd,backupLog,strlen(backupLog));
		printf("\"%s\" removed by \"%s\"\n",tempNode->backupPath,tempNode->originPath);
	}
	return 1;
}
//-d 옵션의 remove를 하기 위한 함수이다.
int RemoveDir(char *path,int option){
	//option이 들어오면 a옵션도 하는 것으로 생각함
	struct dirent **namelist;
	struct stat statbuf;
	struct stat tempbuf;
	int cnt;
	char *realPath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *tempPath = (char *)malloc(sizeof(char *)*PATHMAX);
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path,realPath)==0){
		if(path[0] =='.'){
			path++;
			sprintf(realPath,"%s%s",exePATH,path);
		}
		else{
			strcpy(realPath,path);
		}
	}
	if(lstat(realPath,&tempbuf)<0){
		fprintf(stderr,"lstat cause error.\n");
		exit(1);
	}
	if(!S_ISDIR(tempbuf.st_mode)){
		printf("ERROR: \'%s\' is not directory path.\n",path);
		exit(1);
	}
	if((cnt = scandir(realPath,&namelist,NULL,alphasort)) == -1){
		fprintf(stderr,"ERROR: cannot scandir.\n");
		exit(1);
	}
	//scandir함수를 통해서 디렉토리 안의 파일과 디렉토리를 확인한다.
	for(int i = 0 ; i <cnt ;i++){
	if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
	 strcpy(tempPath,realPath);
	 strcat(tempPath,"/");
	 strcat(tempPath,namelist[i]->d_name);
	 if(lstat(tempPath,&statbuf)<0){
		 fprintf(stderr,"ERROR: lstat cause error %s\n",tempPath);
		 exit(1);
	 }
	 if(S_ISDIR(statbuf.st_mode)) continue;

	 //이제 여기서 백업으로 만들었던 것들 비교하면서 맞는지 확인하면 된다. 이때 만든 백업노드를 그냥 이용하자.

	 if(S_ISREG(statbuf.st_mode)){
		if(option == 0){
		 RemoveFile(tempPath);
		}
		if(option ==1){
			RemoveForA(tempPath);
		}
	 }//file인 경우에 옵션 -a의 유무에 따라 다른 함수로 removefile을 진행한다.
	 
	}
	return 1;
}
//-r옵션의 remove를 위한 함수이다.
int RemoveForR(char *path, int option){
	dirNode *head = (dirNode *)malloc(sizeof(dirNode));
	char *realPath = (char *)malloc(sizeof(char*)*PATHMAX);
	char *dateData = (char *)malloc(sizeof(char*)*14);
	char *spareData = (char*)malloc(sizeof(char*)*PATHMAX);
	char *resultPath = (char*)malloc(sizeof(char*)*PATHMAX);
	int cnt;
	struct dirent **namelist;
	struct stat statbuf;
	mainDirList = (dirList *)malloc(sizeof(dirList));
	queue* Queue = (queue*)malloc(sizeof(queue));
	Queue->first = NULL;
	Queue->last = NULL;
	mainDirList -> head = head;
	head->next = NULL;
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path,realPath)==NULL){
		printf("realpath cause error.\n");
	}
	if(lstat(realPath,&statbuf)<0){
		fprintf(stderr,"lstat cuase error.\n");
		exit(1);
	}
	if(!S_ISDIR(statbuf.st_mode)){
		printf("디렉토리 경로가 아닙니다.\n");
		exit(1);
	}
	backupRecursive(Queue,realPath,1);
	//이제 탐색 끝냈으니까 mainDirList에서 하나씩 꺼내면서 출력하면 되는 것이다.
	dirNode* tempNode = mainDirList->head;
	fileNode *tempFileNode;
	dateData = getDate();
	

	while(tempNode->next!=NULL){
		tempNode = tempNode->next;
		tempFileNode = tempNode->filehead;
		strcpy(spareData,tempNode->path);
		//디렉토리 경로를 얻기 위함이다.
		char *dirData = (char *)malloc(sizeof(char*)*PATHMAX);
		char *result = strstr(spareData,exePATH);
		memmove(dirData, result +strlen(exePATH),strlen(result + strlen(exePATH))+1);
		dirData++;
		result = strstr(dirData,"/");
		sprintf(dirData,"%s",result);
		//dirData를 통해 그 부분의 구한 디렉토리 부분의 문자열을 얻는다.
		if(result==NULL){
			dirData ="";
		}
		while(tempFileNode->next!=NULL){
			tempFileNode = tempFileNode->next;
			if(option == 0){
				RemoveFile(tempFileNode->path);
			}
			if(option ==1){
				RemoveForA(tempFileNode->path);
			}//옵션 -a의 유무에 따라 다른 remove함수를 수행시킨다.
		}
	}
	return 1;
}

//recover를 수행하기 위한 함수이다.
int RecoverFile(char *path,int option){
	//backup 파일을 통해 원래파일을 복구 시키는 것이다.
	int num;
	int count =1 ;
	int fd,fd1,fd2,fd3;
	int idx;
	int len;
	struct stat statbuffer,checkbuffer;
	struct stat tempbuffer;
	char *buf =(char *)malloc(sizeof(char *)*STRMAX);
	char *originPath = (char *)malloc(sizeof(char *) * PATHMAX);
	backupNode *head = (backupNode *)malloc(sizeof(backupNode));
	char *dateData = (char *)malloc(sizeof(char *)*14);
	backupNode *tempNode =(backupNode *)malloc(sizeof(backupNode));
	char *forPrintChar=(char *)malloc(sizeof(char *)*STRMAX);
	char *emptyChar = (char *)malloc(sizeof(char *)*STRMAX);
	char *tempChar= (char *)malloc(sizeof(char*)*STRMAX);
	char *removeDirPath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *filepath =(char *)malloc(sizeof(char *)*PATHMAX);
	char *backupLog = (char *)malloc(sizeof(char *)* PATHMAX *2 +10);
	char *fileHash = (char *)malloc(sizeof(char *)*hash);
	char *matchHash = (char *)malloc(sizeof(char *)*hash);
	char *fileBackupPath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *findPath = (char *)malloc(sizeof(char *)*PATHMAX);

	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path,originPath)==NULL){
		if(path[0] =='.'){
			fileBackupPath=strchr(path,'/');
			fileBackupPath++;
			sprintf(findPath,"/home/backup/%s",fileBackupPath);
			
		sprintf(originPath,"%s/%s",exePATH,fileBackupPath);
		} //상대 경로인 경우
		else{
			sprintf(originPath,"%s",path);
		}//절대경로인 경우
	 
	}
	head = searchNode(forFileList->head,originPath);
	//입력으로 넣어준 path의 originPath를 구해서 그 path와 같은 originPath를 가진 노드들의 링크드리스트를 리턴받는다.
	if(head->next == NULL){
	if(lstat(originPath,&tempbuffer)<0){
		fprintf(stderr,"ERROR: this is not existing path.\n");
		exit(1);
	}
	if(S_ISDIR(tempbuffer.st_mode)){
		printf("ERROR : this is directory path\n");
		exit(1);
	}
	}//링크드리스트에 내용이 없는 경우에는 파일이 경로가 존재하는지 확인하고 존재하는 경우 디렉토리인지 확인한다.
	head = searchNode(forFileList->head ,originPath);
	//원래 경로를 통해서 새로운 head에 링크드리스트 연결시켜줌
	tempNode = head;
	strcpy(filepath, originPath);
	for(idx = strlen(filepath)-1; filepath[idx] != '/'; idx--);
	filepath[idx] = '\0';
    
	if(access(filepath,F_OK)){
             while(mkdir(filepath, 0777)<0){
             //mkdir이 실패하였을 경우
                 accessDirCheck(filepath);
             }//파일을 생성할 수 있을 때까지 반복해준다.
         }

	if(tempNode == NULL){
		printf("No file\n");
		exit(1);
	}
	if(option==1){
		while(tempNode->next->next!=NULL){
			tempNode = tempNode->next;
		}
	}//-l옵션이 들어온 경우에는 최신의 백업파일로 recover해야하기 때문에 최신의 backupfile로 이동시켜주는 것이다.

	if(tempNode->next==NULL){
		printf("ERROR: there is no backup file.\n");
		return 1;
	}//backupfile이 한 개도 존재하지 않을 경우
	if(tempNode ->next->next==NULL){
		tempNode = tempNode ->next;
		if(lstat(originPath,&checkbuffer)==0){
			md5(originPath,fileHash);
		
			md5(tempNode->backupPath,matchHash);
				if(strcmp(fileHash,matchHash)==0){
					printf("\"%s\" not changed with \"%s\"\n",tempNode->backupPath,originPath);
					return 1;
					}
		}//md5값을 통해 recover하려는 backupfile이 기존 파일과 같은지 확인해주고 그 결가에 다른 출력을 해준다.

		if((fd1 = open(tempNode->backupPath,O_RDONLY))<0){
			fprintf(stderr,"open file cause error\n");
			exit(1);
		}
		if((fd2 = open(tempNode->originPath, O_RDWR|O_CREAT|O_TRUNC,0777))<0){
			fprintf(stderr ,"open file cause error.\n");
			exit(1);
		}
		if(fchmod(fd2,0777)<0){
			fprintf(stderr,"fchmod error\n");
			exit(1);
		}
		if(lstat(tempNode->backupPath,&statbuffer)<0){
			fprintf(stderr,"lstat error.\n");
			exit(1);
		}
		while((len = read(fd1, buf, statbuffer.st_size))>0){
			write(fd2,buf,len);
		}//백업 파일을 통해 작성해주는 것이다.
		//백업해주고 없으면 삭제하는 부분
		if(remove(tempNode->backupPath)	!=0){
			printf("ERROR: cannot remove file.\n");
		}
		strcpy(removeDirPath,tempNode->backupPath);
		//dir이 비어있는지 검사하고 삭제하기 위해서 사용함.
		tempChar=strstr(removeDirPath,"backup");
		strncpy(forPrintChar,tempChar + 7, 12);
		sprintf(emptyChar,"%s/%s",backupPATH,forPrintChar);
		removeEmptyDir(emptyChar);
		//비어있는 디렉토리는 삭제해준다.
		dateData = getDate();
		if((fd3 = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
			fprintf(stderr, "open file cause error3\n");
			exit(1);
		}
		sprintf(backupLog, "%s : \"%s\" recovered to \"%s\"\n",dateData,tempNode ->backupPath, tempNode -> originPath);
		write(fd3,backupLog,strlen(backupLog));
		printf("\"%s\" recovered to \"%s\"\n",tempNode->backupPath, tempNode->originPath);
		return 1;
	}
	printf("backup files of %s\n",originPath);
	printf("0. exit\n");
	tempNode = head;
	while(tempNode->next !=NULL){
		
			printf("%d. ",count++);
			
			tempChar=strstr(tempNode->next->backupPath,"backup");
			strncpy(forPrintChar, tempChar + 7, 12);
			if(lstat(tempNode->next->backupPath,&statbuffer)<0){
				fprintf(stderr,"lstat error\n");
				exit(1);
			}
			//파일 크기까지 읽는 함수
			printf("%s            %ldbytes\n",forPrintChar,statbuffer.st_size);
			tempNode = tempNode->next;
		} //백업 파일 목록들을 출력시켜주는 것이다.
	printf(">>");
	scanf("%d",&num);
	//recover할 backupfile의 번호를 입력받는다.
	if(num ==0)
			return 1;
	else if(num+1 > count|num<0){
			printf("This is wrong number.\n");
			return 1;
		}//잘못된 번호를 입력하였을 때의 에러처리이다.
	tempNode = head;
	for(int i =0; i<num;i++){
			tempNode = tempNode ->next;
		}
		//노드를 이동시켜서 적어주는 것이다. 만약에 md5로 해싱했을 때 값이 같으면 안해도됨
		if(lstat(tempNode->originPath,&checkbuffer)==0){
		md5(tempNode->originPath,fileHash);
		md5(tempNode->backupPath,matchHash);

		if(strcmp(fileHash,matchHash)==0){
			printf("\"%s\" not changed with \"%s\"\n",tempNode->backupPath,originPath);
			return 1;
		}
		}
		if((fd1 = open(tempNode->backupPath,O_RDONLY))<0){
		fprintf(stderr,"open file cause error\n");
		exit(1);
		}
		if((fd2 = open(tempNode->originPath, O_CREAT|O_TRUNC|O_WRONLY,0777))<0){
		fprintf(stderr ,"open file cause error\n");
		exit(1);
		}
		if(fchmod(fd2,0777)<0){
			fprintf(stderr,"fchmod error\n");
			exit(1);
		}
		while((len = read(fd1, buf, statbuffer.st_size))>0){
			write(fd2,buf,len);
		}//이제 다 적어준 것이다.

		if(remove(tempNode->backupPath)	!=0){
			printf("파일을 삭제하는데 실패함\n");
		}
		strcpy(removeDirPath,tempNode->backupPath);
		//dir이 비어있는지 검사하고 삭제하기 위해서 사용함.
		tempChar=strstr(removeDirPath,"backup");
		strncpy(forPrintChar,tempChar + 7, 12);
		sprintf(emptyChar,"%s/%s",backupPATH,forPrintChar);
		removeEmptyDir(emptyChar);
		//없으면 디렉토리를 삭제
		dateData = getDate();
		if((fd3 = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
			fprintf(stderr, "open file cause error3\n");
			exit(1);
		}
		sprintf(backupLog, "%s : \"%s\" recovered to \"%s\"\n",dateData,tempNode->backupPath, tempNode->originPath);
		write(fd3,backupLog,strlen(backupLog));
		printf("\"%s\" recovered to \"%s\"\n",tempNode->backupPath, tempNode->originPath);
}
//-d 옵션을 사용하기 위한 recover함수이다.
int RecoverDir(char *path,int option){
	//option이 들어오면 a옵션도 하는 것으로 생각함
	struct dirent **namelist;
	struct stat statbuf;
	int cnt;
	char *realPath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *tempPath = (char *)malloc(sizeof(char *)*PATHMAX);
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path,realPath)==0){
		printf("realpath에서 에러가 발생.\n");
	}
	
	if((cnt = scandir(realPath,&namelist,NULL,alphasort)) == -1){
		fprintf(stderr,"directory를 열 수 없습니다\n");
		exit(1);
	}
	for(int i = 0 ; i <cnt ;i++){
	if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
	 strcpy(tempPath,realPath);
	 strcat(tempPath,"/");
	 strcat(tempPath,namelist[i]->d_name);
	 if(lstat(tempPath,&statbuf)<0){
		 fprintf(stderr,"lstat에서 오류발생\n");
		 exit(1);
	 }
	 if(S_ISDIR(statbuf.st_mode)) continue;


	 if(S_ISREG(statbuf.st_mode)){
		if(option == 0){
		 RecoverFile(tempPath,0);
		}
		if(option ==1){
			RecoverFile(tempPath,1);
		}//file인 경우 옵션 0과,1을 넣어줘서 a옵션을 사용하지 않고 사용하는 것으로 구분한다.
	 }
	 
	}
	return 1;
}

//-r옵션을 사용하는 recover함수를 위한 것이다.
int RecoverForR(char *path, int option){
	dirNode *head = (dirNode *)malloc(sizeof(dirNode));
	char *realPath = (char *)malloc(sizeof(char*)*PATHMAX);
	char *dateData = (char *)malloc(sizeof(char*)*14);
	char *spareData = (char*)malloc(sizeof(char*)*PATHMAX);
	char *resultPath = (char*)malloc(sizeof(char*)*PATHMAX);
	int cnt;
	struct dirent **namelist;
	struct stat statbuf;
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	mainDirList = (dirList *)malloc(sizeof(dirList));
	queue* Queue = (queue*)malloc(sizeof(queue));
	Queue->first = NULL;
	Queue->last = NULL;
	mainDirList -> head = head;
	head->next = NULL;
	if(realpath(path,realPath)==NULL){
		printf("realpath cause error.\n");
	}
	if(lstat(realPath,&statbuf)<0){
		fprintf(stderr,"lstat cuase error.\n");
		exit(1);
	}
	if(!S_ISDIR(statbuf.st_mode)){
		printf("디렉토리 경로가 아닙니다.\n");
		exit(1);
	}
	backupRecursive(Queue,realPath,1);
	//mainDirList를 얻기 위해서 함수를 BFS를 재귀적 함수로 수행시켜 준다.
	dirNode* tempNode = mainDirList->head;
	fileNode *tempFileNode;
	dateData = getDate();

	while(tempNode->next!=NULL){

		tempNode = tempNode->next;
		tempFileNode = tempNode->filehead;

		while(tempFileNode->next!=NULL){
			tempFileNode = tempFileNode->next;
			RecoverFile(tempFileNode->path,option);
		}//directory경로를 이동하면서 파일들을 recover
	}
	return 1;
}


//backup명령어에서 r 옵션을 사용할때 디렉토리 위치와 날짜를 같게 하기 위해서 만든 함수입니다.	
int BackupfileForR(char *path, int option,char* dateData,char* dirData){
	int len;	
    int fd1, fd2, fd3;
    char *buf = (char *)malloc(sizeof(char *) * STRMAX);
    struct stat statbuf, tmpbuf;
    struct dirent **namelist;
    struct dirent **namelist2;
    int cnt,cnt2;
    char *filename = (char *)malloc(sizeof(char *) * PATHMAX);
    char *filepath = (char *)malloc(sizeof(char *) * PATHMAX);
    char *tmpPath = (char *)malloc(sizeof(char *) * PATHMAX);
    char *tmpPath2 = (char *)malloc(sizeof(char *) * PATHMAX);
	char *tmpName = (char *)malloc(sizeof(char *) * PATHMAX);
    char *tmpdir = (char *)malloc(sizeof(char *) * PATHMAX);
    char *newPath = (char *)malloc(sizeof(char *) * PATHMAX);
	char *forMakePath = (char *)malloc(sizeof(char *) * PATHMAX);
	int idx;
	char *dirNewPath = (char *)malloc(sizeof(char *)*PATHMAX);
    char *filehash = (char *)malloc(sizeof(char *) * hash);
    char *tmphash = (char *)malloc(sizeof(char *) * hash);
	char *realPATH = (char *)malloc(sizeof(char *)* PATHMAX);
	char *backupLog = (char *)malloc(sizeof(char *)* PATHMAX *2 +10);
	backupNode* head = (backupNode*)malloc(sizeof(backupNode));
	backupNode* tempNode = (backupNode *)malloc(sizeof(backupNode));
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path, realPATH)==NULL){
         printf("realpath error\n");
     }
	strcpy(filepath,realPATH);
	for(idx = strlen(filepath)-1; filepath[idx] != '/'|idx==0; idx--);
    strcpy(filename, filepath+idx+1);
    filepath[idx] = '\0';
	
    if (lstat(realPATH, &statbuf) < 0) {
       fprintf(stderr, " lstat cause error %s\n", path);
       exit(1);
	 }
	md5(realPATH, filehash);
	head = forFileList -> head;
	tempNode = head;

	while(tempNode->next!= NULL){
		tempNode= tempNode->next;
		if(strcmp(tempNode->originPath,realPATH)==0){	
	
		if(option == 0 ){
	
			md5(tempNode->backupPath,tmphash);
			if(!strcmp(filehash,tmphash)){
					printf("\"%s\" is already backuped to \"%s\"\n",realPATH,tempNode->backupPath);
					return 1;
			}	
	}
	}
	}
	//backupNode중에 같은게 있으면 같은게 있다고 출력하여준다.
	if(!access("/home/backup",F_OK)){
		sprintf(dirNewPath,"%s/%s",backupPATH,dateData);
		if(access(dirNewPath,F_OK)){
			mkdir(dirNewPath,0777);
		}
		sprintf(newPath,"%s/%s%s",backupPATH,dateData,dirData);
		if(access(newPath,F_OK)){
			while(mkdir(newPath, 0777)<0){
			//mkdir이 실패하였을 경우
				accessDirCheck(newPath);
			}//파일을 접근할 수 있을 때까지 반복해준다.
		}
	}
	if((fd1 = open(realPATH,O_RDONLY))<0){
		fprintf(stderr,"open file cause error\n");
		exit(1);
	}
	sprintf(forMakePath,"%s/%s",newPath,filename);
	if((fd2 = open(forMakePath, O_CREAT|O_TRUNC|O_WRONLY,0777))<0){
		fprintf(stderr ,"open file cause error22\n");
		exit(1);
	}
	
	while((len = read(fd1, buf, statbuf.st_size))>0){
			write(fd2,buf,len);

	}

	printf("\"%s\" backuped to \"%s\"\n",realPATH, forMakePath);

	if((fd3 = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
		fprintf(stderr, "open file cause error3\n");
		exit(1);
	}
	sprintf(backupLog, "%s : \"%s\" backuped to \"%s\"\n",dateData,realPATH, forMakePath);
	write(fd3,backupLog,strlen(backupLog));
	close(fd1);
	close(fd2);
}



//파일을 backupdir안에 넣어 백업해주는 함수다.
int BackupFile(char *path, int option){
	
    int len;
    int fd1, fd2, fd3;
    char *buf = (char *)malloc(sizeof(char *) * STRMAX);
    struct stat statbuf, tmpbuf;
    struct dirent **namelist;
    struct dirent **namelist2;
    int cnt,cnt2;
	char *dateData = (char *)malloc(sizeof(char *)* 14);
    char *filename = (char *)malloc(sizeof(char *) * PATHMAX);
    char *filepath = (char *)malloc(sizeof(char *) * PATHMAX);
    char *tmpPath = (char *)malloc(sizeof(char *) * PATHMAX);
    char *tmpPath2 = (char *)malloc(sizeof(char *) * PATHMAX);
	char *tmpName = (char *)malloc(sizeof(char *) * PATHMAX);
    char *tmpdir = (char *)malloc(sizeof(char *) * PATHMAX);
    char *newPath = (char *)malloc(sizeof(char *) * PATHMAX);
	char *forMakePath = (char *)malloc(sizeof(char *) * PATHMAX);
	int idx;	
    char *filehash = (char *)malloc(sizeof(char *) * hash);
    char *tmphash = (char *)malloc(sizeof(char *) * hash);
	char *realPATH = (char *)malloc(sizeof(char *)* PATHMAX);
	char *backupLog = (char *)malloc(sizeof(char *)* PATHMAX *2 +10);
	backupNode* head = (backupNode*)malloc(sizeof(backupNode));
	backupNode* tempNode = (backupNode *)malloc(sizeof(backupNode));
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path, realPATH)==NULL){
         printf("ERROR: \'%s\' is not exist\n",realPATH);
		 exit(1);
     }
	if(strstr(realPATH,"/home/backup")){
		printf("ERROR: path must be in user directory\n");
		printf("-\'%s\' is not in user directory\n",realPATH);
		exit(1);
	}
	if(strstr(realPATH,"/home")==NULL){
		printf("ERROR: path must be in user directory\n");
		printf("- \'%s\' is not in user directoy\n",realPATH);
		exit(1);
	}
	strcpy(filepath,realPATH);
	for(idx = strlen(filepath)-1; filepath[idx] != '/'|idx==0; idx--);
	//상대 경로이던 절대경로이던간에 마지막을 기준으로 filename을 가져온다.
    strcpy(filename, filepath+idx+1);
    filepath[idx] = '\0';
	//filepath에서 파일 이름부분을 제외하고 넘기는 것이다.
    if (lstat(realPATH, &statbuf) < 0) {
       fprintf(stderr, " ERROR: can't access %s\n", path);
       exit(1);
	 }

	if(S_ISDIR(statbuf.st_mode)){
		printf("ERROR: \'%s\' is directory.\n",realPATH);
		printf("- use \'-d\' option or input file path\n");
		exit(1);
	}
	md5(realPATH, filehash);
	head = forFileList -> head;
	tempNode = head;

	while(tempNode->next!= NULL){
		tempNode= tempNode->next;
		if(strcmp(tempNode->originPath,realPATH)==0){	
	
		if(option == 0 ){
			md5(tempNode->backupPath,tmphash);
			if(!strcmp(filehash,tmphash)){
					printf("\"%s\" is already backuped to \"%s\"\n",realPATH,tempNode->backupPath);
					return 1;
			}
		
	}
	}
	}
	//backup하려는 파일의 내용이 같으면 backup을 진행하지 않는 것을 md5함수의 해시값 비교를 통해 확인한다.
	dateData =	getDate(); //지금의날짜정보를 입력받아서 저장
	

	if(!access("/home/backup",F_OK)){
		sprintf(newPath,"%s/%s",backupPATH,dateData);
		mkdir(newPath, 0777);
	}

	if((fd1 = open(realPATH,O_RDONLY))<0){
		fprintf(stderr,"open file cause error\n");
		exit(1);
	}
	sprintf(forMakePath,"%s/%s",newPath,filename);
	if((fd2 = open(forMakePath, O_CREAT|O_TRUNC|O_WRONLY,0777))<0){
		fprintf(stderr ,"open file cause error\n");
		exit(1);
	}
	
	while((len = read(fd1, buf, statbuf.st_size))>0){
			write(fd2,buf,len);

	}
	

	printf("\"%s\" backuped to \"%s\"\n",realPATH, forMakePath);
	

	if((fd3 = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
		fprintf(stderr, "open file cause error3\n");
		exit(1);
	}
	sprintf(backupLog, "%s : \"%s\" backuped to \"%s\"\n",dateData,realPATH, forMakePath);
	write(fd3,backupLog,strlen(backupLog));
	addBackupNode(forFileList->head,realPATH,forMakePath);
	close(fd1);
	close(fd2);
	//백업을 진행하고 이에 대한 내용을 로그에 작성한다.
}
//directory에 대해서 backup을 진행할 때 사용하는 함수이다.
int BackupDir(char * path,int option){
	struct dirent **namelist;
	struct stat statbuf;
	char *tempPath = (char *)malloc(sizeof(char *) * PATHMAX);
	char *tmpdir = (char *)malloc(sizeof(char *) * PATHMAX);
	int cnt,len,fd,fd1,fd2,fd3;
	int switching=0;
	char *tmpHash = (char *)malloc(sizeof(char *)*hash);	
	char *matchHash = (char *)malloc(sizeof(char *)*hash);
	char *dateData = (char *)malloc(sizeof(char *)* 14);
	char *realPath = (char *)malloc(sizeof(char *)*PATHMAX);
	char *forMakePath = (char *)malloc(sizeof(char*)*PATHMAX);
	char *newPath = (char *)malloc(sizeof(char *)*PATHMAX);
	backupNode* tempNode;
	char *backupLog = (char *)malloc(sizeof(char *)* PATHMAX *2 +10);
	char *buf = (char *)malloc(sizeof(char *) * STRMAX);
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	if(realpath(path,realPath)==NULL){
		printf("real path cause error.\n");
		return 1;
	}
	
	if(lstat(realPath,&statbuf)<0){
		fprintf(stderr,"lstat cause error\n");
		exit(1);
	}
	if(!S_ISDIR(statbuf.st_mode)){
		printf("ERROR : \'%s\' is not directory path.\n",path);
		return 1;
	}
	if((cnt = scandir(realPath,&namelist,NULL,alphasort)) == -1){
		fprintf(stderr,"ERROR: cannot open %s\n",realPath);
		exit(1);
	}

	dateData = getDate();
	if(!access("/home/backup",F_OK)){
		sprintf(newPath,"%s/%s",backupPATH,dateData);
		mkdir(newPath, 0777);
	}
	//새로운 디렉토리를 만들어줌

	for(int i= 0 ; i < cnt; i++){
		
	 if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
	 strcpy(tempPath,realPath);
	 strcat(tempPath,"/");
	 strcat(tempPath,namelist[i]->d_name);
	 if(lstat(tempPath,&statbuf)<0){
		 fprintf(stderr,"ERROR: lstat cause error.\n");
		 exit(1);
	 }// 현재 경로에 대한 파일들을 조사하기 위해서 lstat함수를 사용한다.
	 if(S_ISDIR(statbuf.st_mode)) continue;
	 
	 //경로의 파일이 디렉토리인 경우에는 넘어간다.

	 if(S_ISREG(statbuf.st_mode)){
		tempNode = forFileList ->head;
		while(tempNode->next !=NULL){
			tempNode= tempNode->next;
		
			if(strcmp(tempNode->originPath,tempPath)==0){
				md5(tempNode->backupPath,matchHash);
				md5(tempPath, tmpHash);
				if(option==0){
					if(!strcmp(matchHash,tmpHash)){
						printf("\"%s\" is already backuped\n", tempPath);	
						switching = 1;
						break;

				}
				}
				}	
			}//백업하려는 것과 내용이 같은 경우에는 메세지를 출력하고 넘어간다.
		//백업 내용이 있는 경우에는 아래의 내용을 수행하지 않도록 예외처리 해둔다.
		if(switching == 0){
			if((fd1 = open(tempPath,O_RDONLY))<0){
				fprintf(stderr,"open file cause error\n");
				exit(1);
			}
			sprintf(forMakePath,"%s/%s",newPath,namelist[i]->d_name);
			if((fd2 = open(forMakePath, O_CREAT|O_TRUNC|O_WRONLY,0777))<0){
				fprintf(stderr ,"open file cause error\n");
				exit(1);
			}
			while((len = read(fd1, buf, statbuf.st_size))>0){
				write(fd2,buf,len);
			}	
			printf("\"%s\" backuped to \"%s\"\n",tempPath,forMakePath);

		if((fd3 = open("/home/backup/ssubak.log", O_APPEND|O_WRONLY , 0777))<0){
				fprintf(stderr, "open file cause error3\n");
			exit(1);
			}
			sprintf(backupLog, "%s : \"%s\" backuped to \"%s\"\n",dateData,tempPath, forMakePath);
			write(fd3,backupLog,strlen(backupLog));
			addBackupNode(forFileList->head,tempPath,forMakePath);
		}//backup을 진행하고 로그에 작성시켜줍니다.
		switching = 0;
		}	
		}
}

//backup의 -r 옵션을 수행하기 위한 함수입니다.
int BackupForR(char* path,int option){
	dirNode *head = (dirNode *)malloc(sizeof(dirNode));
	char *realPath = (char *)malloc(sizeof(char*)*PATHMAX);
	char *dateData = (char *)malloc(sizeof(char*)*14);
	char *spareData = (char*)malloc(sizeof(char*)*PATHMAX);
	char *resultPath = (char*)malloc(sizeof(char*)*PATHMAX);
	int cnt;
	struct dirent **namelist;
	struct stat statbuf;
	mainDirList = (dirList *)malloc(sizeof(dirList));
	queue* Queue = (queue*)malloc(sizeof(queue));
	if(strlen(path)>4096){
		printf("Path is too long\n");
		exit(1);
	}
	Queue->first = NULL;
	Queue->last = NULL;
	mainDirList -> head = head;
	head->next = NULL;
	if(realpath(path,realPath)==NULL){
		printf("realpath cause error.\n");
	}
	if(lstat(realPath,&statbuf)<0){
		fprintf(stderr,"lstat cuase error.\n");
		exit(1);
	}
	if(!S_ISDIR(statbuf.st_mode)){
		printf("ERROR: \'%s\' is not directory path\n",path);
		exit(1);
	}
	backupRecursive(Queue,realPath,0);
	//backupRecursive 함수를 통해 BNF를 사용 할 수 있습니다
	dirNode* tempNode = mainDirList->head;
	fileNode *tempFileNode;
	dateData = getDate();

	while(tempNode->next!=NULL){
		tempNode = tempNode->next;
		tempFileNode = tempNode->filehead;
		strcpy(spareData,tempNode->path);
		char *dirData = (char *)malloc(sizeof(char*)*PATHMAX);
		char *result = strstr(spareData,exePATH);
		memmove(dirData, result +strlen(exePATH),strlen(result + strlen(exePATH))+1);	
		dirData++;
		result = strstr(dirData,"/");
		sprintf(dirData,"%s",result);
		if(result==NULL){
			dirData ="";
		}
		//dirData를 만들어서 backup디렉토리의 날짜 디렉토리 밑에 새로운 디렉토리를 만들어서 계층구조를 유지하는 백업파일의 구조를 만들 수 있다.
		while(tempFileNode->next!=NULL){
			tempFileNode = tempFileNode->next;
			BackupfileForR(tempFileNode->path,option,dateData,dirData);
		}
		
	}
	return 1;
}



void Init(){ //맨 처음에 실행되는 함수이다.
	char *fname = "/home/backup/ssubak.log";
	int len;
	int fd;
	char *buf =(char *)malloc(sizeof(char *) * STRMAX);
	char *line =(char *)malloc(sizeof(char *) * STRMAX);
	char *bufLine =(char *)malloc(sizeof(char *) * STRMAX);
	char *bufLine2 =(char *)malloc(sizeof(char *) * STRMAX);
	char *originPathBuf = (char *)malloc(sizeof(char *)*PATHMAX);
	char *cpLine =(char *)malloc(sizeof(char *) * STRMAX);
	char *backupPathBuf = (char *)malloc(sizeof(char *)*PATHMAX);
	//경로를 입력받았을 때 저장시키기 위해서 생성해둔 것이다.
	backupNode *head=(backupNode *)malloc(sizeof(backupNode));
	forFileList = (fileList *)malloc(sizeof(fileList));//메모리갖기위해 동적할당
	forFileList ->head = head;
	backupNode *new = (backupNode *)malloc(sizeof(backupNode));
		
	struct stat statbuf;
	getcwd(exePATH, PATHMAX); //현재 작업디렉토리의 경로를 가져와서 저장한다.
	sprintf(homePATH, "%s", getenv("HOME"));
	
	//backup이 home 뒤에 있는지 확인하고 없으면 생성해준다.
    if(access("/home/backup", F_OK)!=0){
        mkdir("/home/backup", 0777);
   }

	if (chmod("/home/backup",S_IRWXU | S_IRGRP | S_IXGRP | S_IWGRP| S_IROTH | S_IXOTH ) == 0) {	
	  } else {
		printf("ssubak.log file cuase error\n");
	  }
	
	if((fd =open(fname, O_RDWR|O_CREAT,S_IRWXU | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH)) <0){
		printf("error");
		fprintf(stderr,"error cause\n");
		exit(1);
	} //O_CREAT플래그만 달아주는 이유는 로그는 계속 남아있어야하기 때문

	
	sprintf(backupPATH, "/home/backup");
	
	if(lstat("/home/backup/ssubak.log",&statbuf)<0){
		fprintf(stderr, "lstat에서 에러발생\n");
		exit(1);
	}

	int count =0;
	if(statbuf.st_size ==0){
		return;
	}
	

	while((len = read(fd,buf,statbuf.st_size))>0){
		
	
		line = strtok(buf,"\n");
		while(line !=NULL){
			//이 코드를 통해 한 줄 씩 받아 낼 수 있다.
			cpLine = strdup(line);
		
			if(strstr(cpLine,"backuped")!=NULL){
			char *start, *end;
			//첫번째 경로 저장
			start = strchr(cpLine, '\"'); // 시작 따옴표 위치 찾기
			if (start != NULL) {
			end = strchr(start + 1, '\"'); // 시작 따옴표 다음부터 종료 따옴표 위치 찾기
				if (end != NULL) {
			// 부분 문자열 복사
			 strncpy(bufLine, start+1, end - start);
			 bufLine[end - start-1] = '\0'; // 문자열 종료
				}
			}
			 start = strchr(end+1, '\"'); // 시작 따옴표 위치 찾기  
			 if (start != NULL) {
			 end = strchr(start + 1, '\"'); // 시작 따옴표 다음부터 종료 따옴표 위치 찾기
                 if (end != NULL) {         
					 // 부분 문자열 복사
              strncpy(bufLine2, start+1, end - start);
              bufLine2[end - start-1] = '\0'; // 문자열 종료
				 }
			 }
			head=addBackupNode(head,bufLine,bufLine2);
			//backuped가 들어간 로그에 대해서는 링크드리스트에 연결시켜준다.
			forFileList-> head = head;
		}
			else if(strstr(cpLine,"removed")!=NULL){

				char *start, *end;
			//첫번째 경로 저장
				start = strchr(cpLine, '\"'); // 시작 따옴표 위치 찾기
				if (start != NULL) {
				end = strchr(start + 1, '\"'); // 시작 따옴표 다음부터 종료 따옴표 위치 찾기
				if (end != NULL) {
			// 부분 문자열 복사
				 strncpy(bufLine, start+1, end - start);
				 bufLine[end - start-1] = '\0'; // 문자열 종료
					}
				}
				 start = strchr(end+1, '\"'); // 시작 따옴표 위치 찾기  
				 if (start != NULL) {
					 end = strchr(start + 1, '\"'); // 시작 따옴표 다음부터 종료 따옴표 위치 찾기
                 if (end != NULL) {         
					 // 부분 문자열 복사
				  strncpy(bufLine2, start+1, end - start);
					bufLine2[end - start-1] = '\0'; // 문자열 종료
					}	
				 }
				 head = deleteBackupNode(head,bufLine);
				 //removed가 적힌 로그에 대해서는 링크드리스트에서 삭제한다.
				 forFileList -> head = head;
			}

			else if(strstr(cpLine,"recovered")!=NULL){
				char *start, *end;
			//첫번째 경로 저장
				start = strchr(cpLine, '\"'); // 시작 따옴표 위치 찾기
				if (start != NULL) {
				end = strchr(start + 1, '\"'); // 시작 따옴표 다음부터 종료 따옴표 위치 찾기
				if (end != NULL) {
			// 부분 문자열 복사
				 strncpy(bufLine, start+1, end - start);
				 bufLine[end - start-1] = '\0'; // 문자열 종료
					}
				}
				 start = strchr(end+1, '\"'); // 시작 따옴표 위치 찾기  
				 if (start != NULL) {
					 end = strchr(start + 1, '\"'); // 시작 따옴표 다음부터 종료 따옴표 위치 찾기
                 if (end != NULL) {         
					 // 부분 문자열 복사
				  strncpy(bufLine2, start+1, end - start);
					bufLine2[end - start-1] = '\0'; // 문자열 종료
					}	
				 } 
				 head = deleteBackupNode(head,bufLine);
				 //recovered가 적힌 경우에는 backupfile 을 없애기 때문에 링크드 리스트에서 제거한다.
				 forFileList -> head = head;
			}

			if((line = strtok(NULL,"\n"))==NULL) break;
		}
	}

	close(fd);
}

int main(int argc, char *argv[]){	
	Init(); //프로그램을 시작하면 Init함수를 실행시켜준다.
	int option;
	int opt_d=0,opt_r=0,opt_y=0,opt_a=0,opt_l=0,opt_n=0;
	if(argc <2){
		printf("ERROR: wrong input.\n");
		printf("./ssu_backup help : show commands for program\n");
		exit(1);
	}
	//인자가 2개보다 작으면 에러를 출력한다.
	else if(strcmp(argv[1], "help")==0){
		//help명령어가 입력되었을 때의 결과들을 출력한다.
		if(argv[2]){
		if(strcmp(argv[2], "backup")==0){
			printUsage();
			printBackup();
		}
		else if(strcmp(argv[2],"remove")==0){
			printUsage();
			printRemove();
		}else if(strcmp(argv[2],"recover")==0){
              printUsage();
              printRecover();
        }else if(strcmp(argv[2],"list")==0){
              printUsage();
              printList();
        }else if(strcmp(argv[2],"help")==0){
              printUsage();
	          printHelp();
		}else{
			printf("ERROR: You have to input backup, remove, recover, list, help or \"\".\n");
		}
		}
		else{
			printUsage();
			printBackup();
			printRemove();
			printRecover();
			printList();
			printHelp();
		}
	exit(0);
	//backup명령어가  입력되었을 경우
	}else if((strcmp(argv[1],"backup")==0)){
			if(argc < 3){
				printf("ERROR: missing operand <PATH>\n");

				exit(1);
			}
			while((option = getopt(argc,argv,"dry")) != -1){
				if(option == 'd'){
					opt_d =1;
				}

				else if(option == 'r'){
					opt_r =1;
				}
				else if(option == 'y'){
					opt_y =1;
				}
				else{
					printUsage();
					printBackup();
					exit(1);
				}
			
			}
			if(opt_d+opt_r+opt_y ==3){
			if(argv[5]==NULL){
				printUsage();
				printBackup();
			}
			BackupForR(argv[5],1);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==1)&&(opt_y==0)){
			if(argv[4]==NULL){
				printUsage();
				printBackup();
			}
			BackupForR(argv[4],0);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_y==1)){
			if(argv[4]==NULL){
				printUsage();
				printBackup();
			}
			BackupDir(argv[4],1);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_y==1)){
			if(argv[4]==NULL){
				printUsage();
				printBackup();
			}
			BackupForR(argv[4],1);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_y==0)){
			if(argv[3]==NULL){
				printUsage();
				printBackup();
			}
			BackupDir(argv[3],0);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_y==0)){
			if(argv[3]==NULL){
				printUsage();
				printBackup();
			}
			BackupForR(argv[3],0);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_y==1)){
			if(argv[3]==NULL){
				printUsage();
				printBackup();
			}
			BackupFile(argv[3],1);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_y==0)){
			if(argv[2]==NULL){
				printUsage();
				printBackup();
			}
			BackupFile(argv[2],0);
			exit(0);
			}
			

		
	}	//remove명령어가 수행되는 경우
	else if((strcmp(argv[1],"remove")==0)){
			if(argc < 3){
				printf("ERROR: missing operand <PATH>\n");
				exit(1);
			}
			while((option = getopt(argc,argv,"dra"))!=-1){
				if(option == 'd'){
					opt_d =1;
				}

				else if(option == 'r'){
					opt_r =1;
				}
				else if(option == 'a'){
					opt_a =1;
				}
				else{
					printUsage();
					printRemove();
				}
			}
			if(opt_d+opt_r+opt_a ==3){
			if(argv[5]==NULL){
				printUsage();
				printRemove();
			}
			RemoveForR(argv[5],1);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==1)&&(opt_a==0)){
			if(argv[4]==NULL){
				printUsage();
				printRemove();
			}
			RemoveForR(argv[4],0);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_a==1)){
			if(argv[4]==NULL){
				printUsage();
				printRemove();
			}
			RemoveDir(argv[4],1);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_a==1)){
			if(argv[4]==NULL){
				printUsage();
				printRemove();
			}
			RemoveForR(argv[4],1);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_a==0)){
			if(argv[3]==NULL){
				printUsage();
				printRemove();
			}
			RemoveDir(argv[3],0);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_a==0)){
			if(argv[3]==NULL){
				printUsage();
				printRemove();
			}
			RemoveForR(argv[3],0);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_a==1)){
			if(argv[3]==NULL){
				printUsage();
				printRemove();
			}
			RemoveForA(argv[3]);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_a==0)){
			if(argv[2]==NULL){
				printUsage();
				printRemove();
			}
			RemoveFile(argv[2]);
			exit(0);
			}		
	}
	//recover 명령어가 시행되는 경우
	else if((strcmp(argv[1],"recover")==0)){
		//recover에 대한 옵션 처리를 해준다.
		if(argc < 3){
				printf("ERROR: missing operand <PATH>\n");
				exit(1);
			}
		while((option = getopt(argc,argv,"drln"))!=-1){
				if(option == 'd'){
					opt_d =1;
				}
				else if(option == 'r'){
					opt_r =1;
				}
				else if(option == 'l'){
					opt_l =1;
				}
				else if(option ==  'n'){
					opt_n =1;
				}
				else{
					printUsage();
					printRecover();
				}
			}
			if(opt_d+opt_r+opt_l+opt_n ==4){
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==1)&&(opt_l==1)&&(opt_n==0)){
			if(argv[5]==NULL){
				printUsage();
				printRecover();
			}
			RecoverForR(argv[5],1);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==1)&&(opt_l==0)&&(opt_n==1)){
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_l==1)&&(opt_n==1)){
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_l==1)&&(opt_n==1)){
			exit(0);
			}
			//여기까지는 0이 1개
			else if((opt_d ==1)&&(opt_r==1)&&(opt_l==0)&&(opt_n==0)){
			if(argv[4]==NULL){
				printUsage();
				printRecover();
			}
			RecoverForR(argv[4],0);
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_l==0)&&(opt_n==1)){
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_l==1)&&(opt_n==1)){
			exit(0);
			}
			else if((opt_d ==1)&&(opt_r==0)&&(opt_l==1)&&(opt_n==0)){
			if(argv[4]==NULL){
				printUsage();
				printRecover();
			}
			RecoverDir(argv[4],1);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_l==1)&&(opt_n==0)){
			if(argv[4]==NULL){
				printUsage();
				printRecover();
			}
			RecoverForR(argv[4],1);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_l==0)&&(opt_n==1)){
			exit(0);
			}
			//0이 3개 1이 1개인 경우
			else if((opt_d ==1)&&(opt_r==0)&&(opt_l==0)&&(opt_n==0)){
			if(argv[3]==NULL){
				printUsage();
				printRecover();
			}
			RecoverDir(argv[3],0);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==1)&&(opt_l==0)&&(opt_n==0)){
			if(argv[3]==NULL){
				printUsage();
				printRecover();
			}
			RecoverForR(argv[3],0);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_l==1)&&(opt_n==0)){
			if(argv[3]==NULL){
				printUsage();
				printRecover();
			}
			RecoverFile(argv[3],1);
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_l==0)&&(opt_n==1)){
			exit(0);
			}
			else if((opt_d ==0)&&(opt_r==0)&&(opt_l==0)&&(opt_n==0)){
			if(argv[2]==NULL){
				printUsage();
				printRecover();
			}
			RecoverFile(argv[2],0);
			exit(0);
			}
			}
	
	else {//wrong input error.
	printf("ERROR: invalid command -- \'%s\'\n",argv[1]);
	printf("./ssu_backup help : show commands for program\n");
	exit(1);
	}
	return 0;
}
