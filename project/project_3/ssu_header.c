#include "ssu_header.h"


//선언해둔 전역변수들을 정의해줍니다.
char exeNAME[PATHMAX];
char exePATH[PATHMAX];
char homePATH[PATHMAX];
char backupPATH[PATHMAX];
char logPATH[PATHMAX];
char *commanddata[5] = { // 포인터 배열로 변경
  "add", //0
  "remove", //1
  "list",  //2
  "help", //3
  "exit", //4
};

//문자열을 원하는 시작점부터 끝나는 지점까지를 잘라서 리턴시켜주는 함수입니다.
char *substr(char *str, int beg, int end) {
  char *ret = (char*)malloc(sizeof(char) * (end-beg+1));

  for(int i = beg; i < end && *(str+i) != '\0'; i++) {
    ret[i-beg] = str[i];
  }
  ret[end-beg] = '\0';

  return ret;
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
  char *repli = (char *)malloc(sizeof(char)*PATHMAX);
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



//현재시각을 구하는 함수다.
char *getDate() {
	char *date = (char *)malloc(sizeof(char) * 14);
	time_t timer;
	struct tm *t;

	timer = time(NULL);	
	t = localtime(&timer);

  sprintf(date, "%02d%02d%02d%02d%02d%02d",t->tm_year %100, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  
  return date;
}

//daemon프로세스를 만들어주는 함수입니다.
int ssu_daemon_init(char* currentPath,char *fileName,unsigned int time){
  pid_t pid;
  int fd, maxfd;

  if((pid = fork())<0){
    fprintf(stderr,"fork error\n");
    exit(1);
  }
  else if(pid != 0){
	  printf("monitoring started (%s) : %d\n",currentPath,(int)pid);
    exit(0);
  }

  pid = getpid();
  setsid();
  signal(SIGTTIN,SIG_IGN);
  signal(SIGTTOU,SIG_IGN);
  signal(SIGTSTP,SIG_IGN);
  maxfd = getdtablesize();

  for( fd = 0; fd < maxfd; fd++){
    close(fd);
  }
  umask(0);
  chdir("/");
  fd = open("/dev/null",O_RDWR);
  dup(0);
  dup(0);

  pid_t currentPid;
  time_t firstMtime;
  struct stat statbuf;
  struct stat backupBuf;
  struct stat stats;
  bool removeCheck = false;
  char *backupDir = (char *)malloc(sizeof(char)*PATHMAX);
  char *backupLog = (char *)malloc(sizeof(char)*PATHMAX);
  char *currentBuffer= (char *)malloc(sizeof(char)*PATHMAX*2);
  char *lastLine= (char *)malloc(sizeof(char)*PATHMAX*2);
  char *dateBuffer= (char *)malloc(sizeof(char)*20);
  char *gotDate  = (char *)malloc(sizeof(char)*15);
  char *forDateBuffer  = (char *)malloc(sizeof(char)*20);
  char *madePath = (char *)malloc(sizeof(char)*PATHMAX);
  //여기에 로그에 들어갈 내용들을 저장해두는 것입니다.
  char *forWriteLog = (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash1 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash2 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *logBuffer = (char*)malloc(sizeof(char)*PATHMAX*2);
  char *buf = (char *)malloc(sizeof(char)*BUF_MAX);
  FILE *fp;
  FILE *fp2;
  int fd3,fd4;
  int length;
  char *realCurrentPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *realName= (char *)malloc(sizeof(char)*BUF_MAX);

  strcpy(realCurrentPath,currentPath);
  strcpy(realName,fileName);


  currentPid = getpid();

  sprintf(backupDir,"%s/%d",backupPATH,(int)currentPid);
  if(access(backupDir,F_OK)!=0){
      mkdir(backupDir, 0777);
  }

  sprintf(backupLog,"%s/%d.log",backupPATH,(int)currentPid);
  //log파일이 없는 경우에는 만들어줍니다.
  if(access(backupLog,F_OK)!=0){
      int fd2;
      if((fd2 = open(backupLog,O_RDWR|O_CREAT,0777))<0){
        // fprintf(stderr,"open error %s\n",backupLog);
      }
      close(fd2);
  }

  fp = fopen(logPATH,"a+");
  //로그에 새로 등록된 파일을 작성해줍니다.
  sprintf(logBuffer,"%d : %s\n",(int)currentPid,realCurrentPath);
  fwrite(logBuffer,strlen(logBuffer),1,fp);
  //열었던 파일포인터를 닫아줍니다.
  fclose(fp);

  //처음 create에 대한 것을 적어줍니다.
  fp = fopen(backupLog,"a+");
  fseek(fp,0,SEEK_END);
  strcpy(forDateBuffer,makeDate());
  sprintf(forWriteLog,"[%s][create][%s]\n",forDateBuffer,currentPath);
  fwrite(forWriteLog,strlen(forWriteLog),1,fp);
  fclose(fp);
  
  //백업 파일을 만들어서 넣어준다.
  sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
  sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,fileName,gotDate);
  fd3 = open(currentPath,O_RDONLY);
  fd4 = open(madePath,O_RDWR|O_CREAT,0777);
  //파일을 복구하여 줍니다.
  while((length = read(fd3,buf,BUF_MAX))>0){
    write(fd4,buf,length);
  }
  close(fd3);
  close(fd4);
  stat(currentPath,&stats);
  firstMtime = stats.st_mtime;
  
  while(true){

    signal(SIGUSR1,sigusr_handler);

    if(removeCheck==false){
      sleep(time);
      //1초씩 sleep해주면서 과정들을 진행시켜줍니다.
      //먼저 사이즈,mtime비교하고 그 다음에 md5로 해시값을 비교해주면 된다.
      fp2 = fopen(backupLog,"a+");
      fseek(fp2,0,SEEK_SET);
      while(fgets(currentBuffer,PATHMAX*2,fp2)!=NULL){
        strcpy(lastLine,currentBuffer);
      }
      strncpy(dateBuffer,lastLine+1,19);
      dateBuffer[19] = '\0'; // 널 종료 문자 추가
      sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,fileName,dateForm(dateBuffer));
      if(stat(madePath,&backupBuf)<0){
      }
      if(stat(currentPath,&statbuf)<0){
        //삭제된 것이니까 remove된 것으로 보면 된다.
        //삭제되어서 lstat값이 -1로 리턴된 것이다.
        fseek(fp2,0,SEEK_END);
        sprintf(forWriteLog,"[%s][remove][%s]\n",makeDate(),currentPath);
        fwrite(forWriteLog,strlen(forWriteLog),1,fp2);
        removeCheck = true;
        
      }else{
      if(backupBuf.st_size == statbuf.st_size){
        if(firstMtime == statbuf.st_mtime){
          md5(madePath,hash1);
          md5(currentPath,hash2);
          if(strcmp(hash1,hash2)==0){
            //아예 같은 경우이기에 다음과정을 진행해주면 됩니다.
          }else{
              //해쉬값이 다른 경우
              firstMtime = statbuf.st_mtime;
              strcpy(forDateBuffer,makeDate());
              sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
              sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,fileName,gotDate);
              fd3 = open(currentPath,O_RDONLY);
              fd4 = open(madePath,O_RDWR|O_CREAT,0777);
              //파일을 복구하여 줍니다.
              while((length = read(fd3,buf,BUF_MAX))>0){
                write(fd4,buf,length);
              }
              close(fd3);
              close(fd4);
              fseek(fp2,0,SEEK_END);
              sprintf(forWriteLog,"[%s][modify][%s]\n",forDateBuffer,currentPath);
              fwrite(forWriteLog,strlen(forWriteLog),1,fp2);
          }
        }else{
          //mtime이 다른 경우
          firstMtime = statbuf.st_mtime;
          strcpy(forDateBuffer,makeDate());
          sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
          sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,fileName,gotDate);
          fd3 = open(currentPath,O_RDONLY);
          fd4 = open(madePath,O_RDWR|O_CREAT,0777);
          //파일을 복구하여 줍니다.
          while((length = read(fd3,buf,BUF_MAX))>0){
            write(fd4,buf,length);
          }
          close(fd3);
          close(fd4);
          fseek(fp2,0,SEEK_END);
          sprintf(forWriteLog,"[%s][modify][%s]\n",forDateBuffer,currentPath);
          fwrite(forWriteLog,strlen(forWriteLog),1,fp2);
        }
      }else{
        //size가 다른 경우
        firstMtime = statbuf.st_mtime;
        strcpy(forDateBuffer,makeDate());
        sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
        sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,fileName,gotDate);
        fd3 = open(currentPath,O_RDONLY);
        fd4 = open(madePath,O_RDWR|O_CREAT,0777);
        //파일을 백업하여 줍니다.
        while((length = read(fd3,buf,BUF_MAX))>0){
          write(fd4,buf,length);
        }
        close(fd3);
        close(fd4);
        fseek(fp2,0,SEEK_END);
        sprintf(forWriteLog,"[%s][modify][%s]\n",forDateBuffer,currentPath);
        fwrite(forWriteLog,strlen(forWriteLog),1,fp2);     
      }
    }
    fclose(fp2);
    }
    if(removeCheck==true){
      //해당 경로에 파일이 삭제되었다가 생겨난 경우에 대한 처리입니다.
      struct stat liveCheck;
      if(stat(currentPath,&liveCheck)==0){
          fp = fopen(backupLog,"a+");
          fseek(fp,0,SEEK_END);
          strcpy(forDateBuffer,makeDate());
          sprintf(forWriteLog,"[%s][create][%s]\n",forDateBuffer,currentPath);
          fwrite(forWriteLog,strlen(forWriteLog),1,fp);
          fclose(fp);
          
          //백업 파일을 만들어서 넣어준다.
          sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
          sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,fileName,gotDate);
          fd3 = open(currentPath,O_RDONLY);
          fd4 = open(madePath,O_RDWR|O_CREAT,0777);
          //파일을 복구하여 줍니다.
          while((length = read(fd3,buf,BUF_MAX))>0){
            write(fd4,buf,length);
          }
          close(fd3);
          close(fd4);
          stat(currentPath,&stats);
          firstMtime = stats.st_mtime;
          removeCheck = false;
      }
    }
  }
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

//로그에 작성되기 편하게 하기 위한 함수입니다.
char *makeDate(){
  char *date = (char *)malloc(sizeof(char) * 24);
	time_t timer;
	struct tm *t;

	timer = time(NULL);	
	t = localtime(&timer);

  sprintf(date, "%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  
  return date;
}

//날짜 데이터를 14자리의 문자열로 바꿔주는 함수입니다.
char *dateForm(char *dateBuffer){
  char *gotDate = (char *)malloc(sizeof(char) * 21);
  sscanf(dateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
  gotDate[14] = '\0';
  return gotDate;
}

//sigusr가 왔을 때 어떻게 처리할 것인지에 대한 handler입니다.
void sigusr_handler(int signum){
  exit(signum);
}

//디렉토리안에 있는 파일들을 전부 삭제하는 함수입니다.
int deleteDirectory(const char* dirPath) {
   //dir경로를 입력받아서 진행시켜줍니다.
    DIR* dir = opendir(dirPath);

    if (dir == NULL) {
        fprintf(stderr, "Error opening directory: %s\n", dirPath);
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[PATHMAX];
        snprintf(path, PATHMAX, "%s/%s", dirPath, entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf)<0) {
            fprintf(stderr,"stat error %s\n",path);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // 디렉토리인 경우 재귀적으로 삭제
            deleteDirectory(path);
        } else if (S_ISREG(statbuf.st_mode)) {
            // 파일인 경우 삭제
            if (remove(path)<0) {
                fprintf(stderr,"remove error %s\n",path);
                return 1;
            }
        }
    }

    closedir(dir);

    // 마지막으로 빈 디렉토리 삭제
    if (rmdir(dirPath)<0) {
       fprintf(stderr,"rmdir error %s\n",dirPath);
       return 1;
    }
  return 0;
}




//-d 옵션이 생겼을 때 daemon프로세스를 만들어주는 함수입니다.
int ssu_daemon_init_dir(char* currentPath,char *dirName,unsigned int time){
  pid_t pid;
  int fd, maxfd;

  if((pid = fork())<0){
    fprintf(stderr,"fork error\n");
    exit(1);
  }
  else if(pid != 0){
	  printf("monitoring started (%s) : %d\n",currentPath,(int)pid);
    exit(0);
  //데몬 프로세스의 PID를 출력시켜줍니다.
  }
  pid = getpid();
  setsid();
  signal(SIGTTIN,SIG_IGN);
  signal(SIGTTOU,SIG_IGN);
  signal(SIGTSTP,SIG_IGN);
  maxfd = getdtablesize();

  for( fd = 0; fd < maxfd; fd++){
    close(fd);
  }
  umask(0);
  chdir("/");
  fd = open("/dev/null",O_RDWR);
  dup(0);
  dup(0);
  //윗부분이 전부 데몬프로세스를 만들어주는 과정입니다.
  pid_t currentPid;
  time_t firstMtime;
  struct stat statbuf;
  struct stat backupBuf;
  struct stat stats;
  struct stat tempbuf;
  bool removeCheck = false;
  char *backupDir = (char *)malloc(sizeof(char)*PATHMAX);
  char *backupLog = (char *)malloc(sizeof(char)*PATHMAX);
  char *currentBuffer= (char *)malloc(sizeof(char)*PATHMAX*2);
  char *lastLine= (char *)malloc(sizeof(char)*PATHMAX*2);
  char *dateBuffer= (char *)malloc(sizeof(char)*BUF_MAX);
  char *gotDate  = (char *)malloc(sizeof(char)*15);
  char *forDateBuffer  = (char *)malloc(sizeof(char)*BUF_MAX);
  char *madePath = (char *)malloc(sizeof(char)*PATHMAX);
  char *forWriteLog22 = (char *)malloc(sizeof(char)*PATHMAX*3);
  char *hash1 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash2 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *logBuffer = (char*)malloc(sizeof(char)*PATHMAX*2);
  char *buf = (char *)malloc(sizeof(char)*BUF_MAX);
  
  FILE *fp;
  FILE *fp2;
  int fd3,fd4;
  int cnt;
  int length;
  fileNode *fileHead = (fileNode*)malloc(sizeof(fileNode));
  struct dirent **namelist;
  char *tempPath22 = (char *)malloc(sizeof(char)*PATHMAX);
  fileNode *currentNode;

  //pid를 가져와서 저장합니다.
  currentPid = getpid();

  sprintf(backupDir,"%s/%d",backupPATH,(int)currentPid);
  //PID를 통한 디렉토리를 만들어줍니다.
  if(access(backupDir,F_OK)!=0){
      mkdir(backupDir, 0777);
  }

  //log파일이 없는 경우에는 만들어줍니다.
  sprintf(backupLog,"%s/%d.log",backupPATH,(int)currentPid);
  if(access(backupLog,F_OK)!=0){
      int fd2;
      if((fd2 = open(backupLog,O_RDWR|O_CREAT,0777))<0){
      }
      close(fd2);
  }
  //monitor_list.log에 새로 등록된 PID를 작성해줍니다.
  fp = fopen(logPATH,"a+");
  sprintf(logBuffer,"%d : %s\n",(int)currentPid,currentPath);
  fwrite(logBuffer,strlen(logBuffer),1,fp);
  //열었던 파일포인터를 닫아줍니다.
  fclose(fp);

  //fileNode를 만들어서 초기화시켜줍니다.
  fileNode_init(fileHead);
  
  //입력받은 경로에 대한 scandir를 한 후 파일들에 대한 링크드 리시트를 생성해줍니다.
  cnt = scandir(currentPath,&namelist,NULL, alphasort);

  for(int i = 0; i <cnt;i++){
    if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
    sprintf(tempPath22,"%s/%s",currentPath,namelist[i]->d_name);

    if(lstat(tempPath22,&tempbuf)<0){
	  }
    if(S_ISDIR(tempbuf.st_mode))
      continue;
    if(S_ISREG(tempbuf.st_mode)){
      fileNode_insert_forD(fileHead,tempPath22);
    }
  }
  free(tempPath22);

  //처음 create에 대한 것을 적어줍니다.

  strcpy(forDateBuffer,makeDate());

  fp = fopen(backupLog,"a+");
  currentNode = fileHead;
  //처음에 NULL값이 들어있기에 이렇게 설정해줍니다.
  currentNode = currentNode->next_file;
  //위 과정을 전부 반복해줍니다.
  while(currentNode!=NULL){
    fseek(fp,0,SEEK_END);
    sprintf(forWriteLog22,"[%s][create][%s]\n",forDateBuffer,currentNode->file_path);
    fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
  
    //백업 파일을 만들어서 넣어준다.
    sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
    sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currentNode->file_name,gotDate);
    strcpy(currentNode->made_path,madePath);
    fd3 = open(currentNode->file_path,O_RDONLY);
    fd4 = open(madePath,O_RDWR|O_CREAT,0777);
    //파일을 복구하여 줍니다.
    while((length = read(fd3,buf,BUF_MAX))>0){
      write(fd4,buf,length);
    }
    close(fd3);
    close(fd4);
    currentNode = currentNode->next_file;
  }
  fclose(fp);
  free(forWriteLog22);

  while(true){

      signal(SIGUSR1,sigusr_handler);

      //여기에 로그에 들어갈 내용들을 저장해두는 것입니다.
      char *forWriteLog = (char *)malloc(sizeof(char)*PATHMAX*3);

      char *tempPath2 = (char *)malloc(sizeof(char)*PATHMAX);
      //default로 1초 -t옵션의 경우 시간을 입력받아서 해당하는 시간만큼 sleep해줍니다.
      sleep(time);
      
      //디렉토리를 열어서 새로운 파일이 생겼는지 확인하면서 적어나가야 한다.
      //create부분에 대한 확인입니다.
      cnt = scandir(currentPath,&namelist,NULL, alphasort);

      for(int i = 0; i <cnt;i++){
        if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
        if(strstr(namelist[i]->d_name,".swp")!=NULL)continue;
        sprintf(tempPath2,"%s/%s",currentPath,namelist[i]->d_name);
        if(lstat(tempPath2,&tempbuf)<0){
        }
        if(S_ISDIR(tempbuf.st_mode)){
          continue;
        }
        if(S_ISREG(tempbuf.st_mode)){
          //처음 들어온 파일인 경우 
          if(fileNode_get(fileHead,tempPath2) == NULL){
            //로그에 적어주고
            strcpy(forDateBuffer,makeDate());
            fp = fopen(backupLog,"a+");
            fseek(fp,0,SEEK_END);
            sprintf(forWriteLog,"[%s][create][%s]\n",forDateBuffer,tempPath2);
            fwrite(forWriteLog,strlen(forWriteLog),1,fp);
            fclose(fp);

            //링크드리스트에 포함시켜줍니다.
            //이게 잘 안 들어가지는 가진다.
            fileNode_insert_forD(fileHead,tempPath2);
            

            fileNode* hiNode = fileNode_get(fileHead,tempPath2);

            sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
            sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,hiNode->file_name,gotDate);
            strcpy(hiNode->made_path,madePath);
            //백업파일도 만들어줍니다.
            fd3 = open(tempPath2,O_RDONLY);
            fd4 = open(madePath,O_RDWR|O_CREAT,0777);
            //파일을 복구하여 줍니다.
            while((length = read(fd3,buf,BUF_MAX))>0){
              write(fd4,buf,length);
            }
            close(fd3);
            close(fd4);
        }
      }
    }

    //먼저 사이즈,mtime비교하고 그 다음에 md5로 해시값을 비교해주면 됩니다.
    
    //파일노드의 링크드리스트에 대해서 전부 진행시켜줍니다.
    //로그를 열여줍니다.
    fp2 = fopen(backupLog,"a+");
    currentNode = fileHead->next_file;
    while(currentNode!=NULL){
      char *forWriteLog1 = (char *)malloc(sizeof(char)*PATHMAX*3);
      char *forWriteLog2 = (char *)malloc(sizeof(char)*PATHMAX*3);
      char *forWriteLog3 = (char *)malloc(sizeof(char)*PATHMAX*3);
      char *forWriteLog4 = (char *)malloc(sizeof(char)*PATHMAX*3);

    //remove에 대한 부분입니다.
    if(stat(currentNode->file_path,&statbuf)<0){
      //삭제되어서 lstat값이 -1로 리턴된 것이다.
      fseek(fp2,0,SEEK_END);
      sprintf(forWriteLog1,"[%s][remove][%s]\n",makeDate(),currentNode->file_path);
      fwrite(forWriteLog1,strlen(forWriteLog1),1,fp2);
      fileNode *eraseNode = currentNode;
      currentNode = currentNode->next_file;
      //노드를 링크드리스트에서 제거해줍니다.
      fileNode_remove(eraseNode);
      
    }
    else{
      //백업파일과의 stat구조체로 바꿔줍니다.
      //madepath에 대해서 stat함수를 사용해줍니다.

      if(stat(currentNode->made_path,&backupBuf)<0){ 

      }
      //백업해 둔 것과 원본 파일의 사이즈를 비교합니다.
      if(backupBuf.st_size == statbuf.st_size){
        //처음에 저장해뒀던 mtime과 현재 mtime을 비교해줍니다.
        if(currentNode->firstMtime == statbuf.st_mtime){
          md5(currentNode->made_path,hash1);
          md5(currentNode->file_path,hash2);
          //해쉬값을 파일의 내용이 변경되었는지 확이하기 위해서 hash값을 비교해줍니다.
          if(strcmp(hash1,hash2)==0){
            //아예 같은 경우이기에 다음과정을 진행해주면 됩니다.
          }else{
              //해쉬값이 다른 경우
              //mtime을 새로 정의해줍니다.
              currentNode->firstMtime = statbuf.st_mtime;
              strcpy(forDateBuffer,makeDate());
              sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
              sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currentNode->file_name,gotDate);
              strcpy(currentNode->made_path,madePath);
              //파일을 복구하여 줍니다.
              fd3 = open(currentNode->file_path,O_RDONLY);
              fd4 = open(madePath,O_RDWR|O_CREAT,0777);
              
              while((length = read(fd3,buf,BUF_MAX))>0){
                write(fd4,buf,length);
              }
              close(fd3);
              close(fd4);
              fseek(fp2,0,SEEK_END);
              sprintf(forWriteLog2,"[%s][modify][%s]\n",forDateBuffer,currentNode->file_path);
              fwrite(forWriteLog2,strlen(forWriteLog2),1,fp2);
          }
        }else{
          //mtime이 다른 경우
          currentNode->firstMtime = statbuf.st_mtime;
          strcpy(forDateBuffer,makeDate());
          sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
          sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currentNode->file_name,gotDate);
          strcpy(currentNode->made_path,madePath);

          fd3 = open(currentNode->file_path,O_RDONLY);
          fd4 = open(madePath,O_RDWR|O_CREAT,0777);
          //파일을 복구하여 줍니다.
          while((length = read(fd3,buf,BUF_MAX))>0){
            write(fd4,buf,length);
          }
          close(fd3);
          close(fd4);
          fseek(fp2,0,SEEK_END);
          sprintf(forWriteLog3,"[%s][modify][%s]\n",forDateBuffer,currentNode->file_path);
          fwrite(forWriteLog3,strlen(forWriteLog3),1,fp2);
        }
      }else{
        //size가 다른 경우
        currentNode->firstMtime = statbuf.st_mtime;
        strcpy(forDateBuffer,makeDate());
        sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
        sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currentNode->file_name,gotDate);
        strcpy(currentNode->made_path,madePath);

        fd3 = open(currentNode->file_path,O_RDONLY);
        fd4 = open(madePath,O_RDWR|O_CREAT,0777);
        //파일을 복구하여 줍니다.
        while((length = read(fd3,buf,BUF_MAX))>0){
          write(fd4,buf,length);
        }
        close(fd3);
        close(fd4);
        fseek(fp2,0,SEEK_END);
        sprintf(forWriteLog4,"[%s][modify][%s]\n",forDateBuffer,currentNode->file_path);
        fwrite(forWriteLog4,strlen(forWriteLog4),1,fp2);
      }
    }
    if(currentNode==NULL){
      continue;
    }else{
    currentNode = currentNode->next_file;
    }
    free(forWriteLog1);
    free(forWriteLog2);
    free(forWriteLog3);
    free(forWriteLog4);
    }
    fclose(fp2);
    free(forWriteLog);
    free(tempPath2);
  }
}

//parent경로를 구해서 리턴시켜줍니다.
char *getParentPath(char *path){
  char *parentPath = (char*)malloc(sizeof(char)*PATHMAX);
  int idx;
  for( idx = strlen(path); path[idx] != '/'; idx--);
  strncpy(parentPath,path,idx);
  parentPath[idx] = '\0';
  return parentPath;

}


//queue를 연결하는 함수이다.
void enqueue(queue* Queue, char* dirPath){
	queue *newQue = (queue *)malloc(sizeof(queue));
	strcpy(newQue->path,dirPath);
	newQue->next=NULL;
	if(Queue->first == NULL){
		Queue->first = newQue;
		Queue->last = newQue;
		return;
	}
	else{
		queue *tempque;
		tempque = Queue->last->next;
		Queue ->last->next = newQue;
		newQue -> next = tempque;
		Queue -> last = newQue;
    
		return;
	}
}

//큐에서 맨앞에 들어온 node를 빼는 함수이다.
char*  dequeue(queue* Queue){
	queue *tempNode;
	tempNode = Queue->first;
	Queue->first = Queue->first->next;

	return tempNode->path;
} 


//-r 옵션이 생겼을 때 daemon프로세스를 만들어주는 함수입니다.
int ssu_daemon_init_dirR(char* currentPath,char *dirName,unsigned int time){
  pid_t pid;
  int fd, maxfd;

  if((pid = fork())<0){
    fprintf(stderr,"fork error\n");
    exit(1);
  }
  else if(pid != 0){
	printf("monitoring started (%s) : %d\n",currentPath,(int)pid);
	//데몬 프로세스의 PID를 출력시켜줍니다.
	exit(0);
  }
  
  pid = getpid();

  setsid();
  signal(SIGTTIN,SIG_IGN);
  signal(SIGTTOU,SIG_IGN);
  signal(SIGTSTP,SIG_IGN);
  maxfd = getdtablesize();

  for( fd = 0; fd < maxfd; fd++){
    close(fd);
  }
  umask(0);
  chdir("/");
  fd = open("/dev/null",O_RDWR);
  dup(0);
  dup(0);
  //윗부분이 전부 데몬프로세스를 만들어주는 과정입니다.
  pid_t currentPid;
  time_t firstMtime;
  struct stat statbuf;
  struct stat backupBuf;
  struct stat stats;
  struct stat tempbuf;
  bool removeCheck = false;
  char *backupDir = (char *)malloc(sizeof(char)*PATHMAX);
  char *backupLog = (char *)malloc(sizeof(char)*PATHMAX);
  char *currentBuffer= (char *)malloc(sizeof(char)*PATHMAX*2);
  char *lastLine= (char *)malloc(sizeof(char)*PATHMAX*2);
  char *dateBuffer= (char *)malloc(sizeof(char)*BUF_MAX);
  char *gotDate  = (char *)malloc(sizeof(char)*15);
  char *forDateBuffer  = (char *)malloc(sizeof(char)*BUF_MAX);
  char *madePath = (char *)malloc(sizeof(char)*PATHMAX);
  char *forWriteLog22 = (char *)malloc(sizeof(char)*PATHMAX*3);
  char *hash1 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash2 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *logBuffer = (char*)malloc(sizeof(char)*PATHMAX*2);
  char *buf = (char *)malloc(sizeof(char)*BUF_MAX);
  
  FILE *fp;
  FILE *fp2;
  int fd3,fd4;
  int cnt;
  int length;

  dirNode *dirHead = (dirNode*)malloc(sizeof(dirNode));

  struct dirent **namelist;
  char *tempPath22 = (char *)malloc(sizeof(char)*PATHMAX);
  fileNode *currentNode;

  //pid를 가져와서 저장합니다.
  currentPid = getpid();

  sprintf(backupDir,"%s/%d",backupPATH,(int)currentPid);
  //PID를 통한 디렉토리를 만들어줍니다.
  if(access(backupDir,F_OK)!=0){
      mkdir(backupDir, 0777);
  }

  //log파일이 없는 경우에는 만들어줍니다.
  sprintf(backupLog,"%s/%d.log",backupPATH,(int)currentPid);
  if(access(backupLog,F_OK)!=0){
      int fd2;
      if((fd2 = open(backupLog,O_RDWR|O_CREAT,0777))<0){
      }
      close(fd2);
  }
  //monitor_list.log에 새로 등록된 PID를 작성해줍니다.
  fp = fopen(logPATH,"a+");
  sprintf(logBuffer,"%d : %s\n",(int)currentPid,currentPath);
  fwrite(logBuffer,strlen(logBuffer),1,fp);
  //열었던 파일포인터를 닫아줍니다.
  fclose(fp);



  //dirNode를 만들어서 초기화시켜줍니다.
  //queue를 만들어 수정해줍니다.
  dirNode_init(dirHead);
  queue *Queue = (queue*)malloc(sizeof(queue));
  Queue->first = NULL;
  Queue->last = NULL;
  Queue->next=NULL;

  //처음 create에 대한 것을 적어줍니다.
  backupRecursive(Queue,currentPath,dirHead,backupLog,currentPid);

  while(true){
      signal(SIGUSR1,sigusr_handler);
      sleep(time);
      backupRecursive(Queue,currentPath,dirHead,backupLog,currentPid);
  }
}
