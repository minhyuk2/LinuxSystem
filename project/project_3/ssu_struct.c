#include "ssu_header.h"

//전역변수를 선언해둔 것입니다.
dirNode* backup_dir_list;

//여기서 backupNode란 list명령어를 사용하기 위해서 쓰이는 구조체입니다.
//backupNode를 초기화해주는 함수입니다.
void backupNode_init(backupNode *backup_node) {
  backup_node->root_version_dir = NULL;
  backup_node->root_file = NULL;

  backup_node->prev_backup = NULL;
  backup_node->next_backup = NULL;
}

//backupNode를 추가해주는 함수입니다.
backupNode *backupNode_insert(backupNode *backup_head, char *backup_time, char *action,char *file_name, char *dir_path) {
  backupNode *curr_backup = backup_head;
  backupNode *next_backup, *new_backup;

  while(curr_backup != NULL) {
    next_backup = curr_backup->next_backup;
    if(next_backup != NULL && !strcmp(backup_time, next_backup->backup_time)) {
      return curr_backup->next_backup;
    } else if(next_backup == NULL || strcmp(backup_time, next_backup->backup_time) < 0) {
      new_backup = (backupNode*)malloc(sizeof(backupNode));
      backupNode_init(new_backup);

      new_backup->root_file = backup_head->root_file;

      strcpy(new_backup->backup_time, backup_time);
      strcpy(new_backup->origin_path, dir_path);
      strcat(new_backup->origin_path, file_name);
      strcpy(new_backup->action,action);

      new_backup->prev_backup = curr_backup;
      new_backup->next_backup = next_backup;
      
      if(next_backup != NULL) {
        next_backup->prev_backup = new_backup;
      }
      curr_backup->next_backup = new_backup;

      backup_head->root_file->backup_cnt++;

      return curr_backup->next_backup;
    }
    curr_backup = curr_backup->next_backup;
  }
  return NULL;
}


//로그를 표현하기 위해서 사용하는 action에 대한 링크드리스트를 만들어주는 함수입니다.
int backup_list_insert(dirNode* dirList, char* backup_time, char* path, char* action) {
  char* ptr;

  path = path;
  if(ptr = strchr(path, '/')) {
    char* dir_name = (char *)malloc(sizeof(char)*PATHMAX);
    dir_name = substr(path, 0, strlen(path) - strlen(ptr));
    dirNode* curr_dir = dirNode_insert(dirList->subdir_head, dir_name, dirList->dir_path,backup_time);
    backup_list_insert(curr_dir, backup_time, ptr+1, action);
    curr_dir->backup_cnt++;

  } else {
    char *file_name = (char *)malloc(sizeof(char)*FILE_MAX);
    strcpy(file_name,path);
    fileNode* curr_file = fileNode_insert(dirList->file_head, file_name, dirList->dir_path,backup_time);
    backupNode_insert(curr_file->backup_head, backup_time,action ,file_name, dirList->dir_path);
  }
  return 0;
}

//fileNode를 처음에 초기화해주는 함수입니다.
void fileNode_init(fileNode *file_node) {
  file_node->root_dir = NULL;

  file_node->backup_cnt = 0;

  file_node->backup_head = (backupNode*)malloc(sizeof(backupNode));
  file_node->backup_head->prev_backup = NULL;
  file_node->backup_head->next_backup = NULL;
  file_node->backup_head->root_file = file_node;

  file_node->prev_file = NULL;
  file_node->next_file = NULL;
}


//list를 출력하기 위해 파일노드를 추가해주는 함수입니다.
fileNode *fileNode_insert(fileNode *file_head, char *file_name, char *dir_path,char *backup_time) {
  fileNode *curr_file = file_head;
  fileNode *next_file, *new_file;
  
  while(curr_file != NULL) {
    next_file = curr_file->next_file;

    if(next_file != NULL && !strcmp(file_name, next_file->file_name)) {
      return curr_file->next_file;
    } else if(next_file == NULL || strcmp(file_name, next_file->file_name) < 0) {
      new_file = (fileNode*)malloc(sizeof(fileNode));
      fileNode_init(new_file);

      new_file->root_dir = file_head->root_dir;
      strcpy(new_file->backup_time,backup_time);
      strcpy(new_file->file_name, file_name);
      strcpy(new_file->file_path, dir_path);
      strcat(new_file->file_path, file_name);

      new_file->prev_file = curr_file;
      new_file->next_file = next_file;
      
      if(next_file != NULL) {
        next_file->prev_file = new_file;
      }
      curr_file->next_file = new_file;

      file_head->root_dir->file_cnt++;

      return curr_file->next_file;
    }
    curr_file = next_file;
  }
  return NULL;
}

//-d옵션을 위해서 fileNode를 추가해주는 함수입니다.
fileNode *fileNode_insert_forD(fileNode *file_head, char *file_path) {
  fileNode *curr_file = file_head;
  fileNode *next_file, *new_file;
  struct stat statbuf;

  while(curr_file != NULL) {
    next_file = curr_file->next_file;

    if(next_file != NULL && !strcmp(file_path, next_file->file_path)) {
      return curr_file->next_file;
    } else if(next_file == NULL || strcmp(file_path, next_file->file_path) < 0) {

      new_file = (fileNode*)malloc(sizeof(fileNode));
      fileNode_init(new_file);
      new_file->root_dir = file_head->root_dir;
      strcpy(new_file->backup_time,"");
      strcpy(new_file->file_name, GetFileName(file_path));
      strcpy(new_file->file_path, file_path);
      new_file->alive = 1; //alive가 1인 경우에는 remove되지 않은 경우인 것입니다.


      if(stat(file_path,&statbuf)<0){
        fprintf(stderr, "stat error \n");
        return NULL;
      }  
      new_file->firstMtime = statbuf.st_mtime;

      new_file->prev_file = curr_file;
      new_file->next_file = next_file;
      
      if(next_file != NULL) {
        next_file->prev_file = new_file;
      }
      curr_file->next_file = new_file;

      return curr_file->next_file;
    }
    curr_file = next_file;
  }
  return NULL;
}


//-r옵션을 위해서 fileNode를 추가해주는 함수입니다.
fileNode *fileNode_insert_forR(fileNode *file_head, char *file_path) {
  fileNode *curr_file = file_head;
  fileNode *next_file, *new_file;
  struct stat statbuf;
  if(curr_file ==NULL){
  }

  while(curr_file != NULL) {
    next_file = curr_file->next_file;

    if(next_file != NULL && !strcmp(file_path, next_file->file_path)) {
      return curr_file->next_file;
    } else if(next_file == NULL || strcmp(file_path, next_file->file_path) < 0) {

      new_file = (fileNode*)malloc(sizeof(fileNode));
      fileNode_init(new_file);
      new_file->root_dir = file_head->root_dir;
      strcpy(new_file->backup_time,"");
      strcpy(new_file->file_name, GetFileName(file_path));
      strcpy(new_file->file_path, file_path);
      new_file->alive = 1; //alive가 1인 경우에는 remove되지 않은 경우인 것입니다.


      if(stat(file_path,&statbuf)<0){
        fprintf(stderr, "stat error \n");
        return NULL;
      }  
      new_file->firstMtime = statbuf.st_mtime;

      new_file->prev_file = curr_file;
      new_file->next_file = next_file;
      
      if(next_file != NULL) {
        next_file->prev_file = new_file;
      }
      curr_file->next_file = new_file;

      return curr_file->next_file;
    }
    curr_file = next_file;
  }
  return NULL;
}


//원하는 파일노드를 찾는 함수입니다.
fileNode *fileNode_get(fileNode *file_head, char *file_path) {
  fileNode *curr_file = file_head->next_file;
  
  while(curr_file != NULL) {
    if(curr_file != NULL && !strcmp(file_path, curr_file->file_path)) {
      return curr_file;
    }
    curr_file = curr_file->next_file;
  }
  return NULL;
}


//제거하고싶은 filenode를 제거해주는 함수입니다.
void fileNode_remove(fileNode *file_node) {
  fileNode *next_file = file_node->next_file;
  fileNode *prev_file = file_node->prev_file;

  if(next_file != NULL) {
    next_file->prev_file = file_node->prev_file;
    prev_file->next_file = file_node->next_file;
  } else {
    prev_file->next_file = NULL;
  }
  

  free(file_node);
}



//dirNode를 초기화해주는 함수입니다.
void dirNode_init(dirNode *dir_node) {
  dir_node->root_dir = NULL;

  dir_node->file_cnt = 0;
  dir_node->subdir_cnt = 0;
  dir_node->backup_cnt = 0;

  dir_node->file_head = (fileNode*)malloc(sizeof(fileNode));
  dir_node->file_head->prev_file = NULL;
  dir_node->file_head->next_file = NULL;
  dir_node->file_head->root_dir = dir_node;

  dir_node->subdir_head = (dirNode*)malloc(sizeof(dirNode));
  dir_node->subdir_head->prev_dir = NULL;
  dir_node->subdir_head->next_dir = NULL;
  dir_node->subdir_head->root_dir = dir_node;


  dir_node->prev_dir = NULL;
  dir_node->next_dir = NULL;
}

//dirNode를 추가해주는 함수입니다.
dirNode *dirNode_insert(dirNode* dir_head, char* dir_name, char* dir_path,char*backup_time) {
  dirNode *curr_dir = dir_head;
  dirNode *next_dir, *new_dir;
  
  while(curr_dir != NULL) {
    next_dir = curr_dir->next_dir;

    if(next_dir != NULL && !strcmp(dir_name, next_dir->dir_name)) {
      return curr_dir->next_dir;
    } else if(next_dir == NULL || strcmp(dir_name, next_dir->dir_name) < 0) {
      new_dir = (dirNode*)malloc(sizeof(dirNode));
      dirNode_init(new_dir);

      new_dir->root_dir = dir_head->root_dir;
      strcpy(new_dir->backup_time,backup_time);
      sprintf(new_dir->dir_name, "%s", dir_name);
      sprintf(new_dir->dir_path, "%s%s/", dir_path, dir_name);

      new_dir->prev_dir = curr_dir;
      new_dir->next_dir = next_dir;
      
      if(next_dir != NULL) {
        next_dir->prev_dir = new_dir;
      }
      curr_dir->next_dir = new_dir;

      dir_head->root_dir->subdir_cnt++;

      return curr_dir->next_dir;
    }
    curr_dir = next_dir;
  }
  return NULL;
}

//-r 옵션인 경우에 dirNode를 추가해주는 함수입니다.
dirNode *dirNode_insert_forR(dirNode* dir_head,fileNode *fileHead, char* dir_path,char *parentPath) {
  dirNode *curr_dir = dir_head;
  dirNode *next_dir, *new_dir;
  
  while(curr_dir != NULL) {
    next_dir = curr_dir->next_dir;

    if(next_dir != NULL && !strcmp(dir_path, next_dir->dir_path)) {
      return curr_dir->next_dir;
    }else if((next_dir != NULL)&&(strcmp(parentPath,next_dir->parent_path)==0)&&(dirNode_get(dir_head,dir_path)==NULL)){
      //level을 맞추기 위해서 부모 경로가 같은 경우에 바로 앞에 추가해준다.
      new_dir = (dirNode*)malloc(sizeof(dirNode));
      dirNode_init(new_dir);

      new_dir->root_dir = dir_head->root_dir;
      
      sprintf(new_dir->dir_name, "%s", GetFileName(dir_path));
      strcpy(new_dir->dir_path,dir_path);
      strcpy(new_dir->backup_time,"");
      //부모의 주소를 적어넣어줍니다.
      strcpy(new_dir->parent_path,parentPath);

      new_dir->file_head = fileHead;

      new_dir->prev_dir = curr_dir;
      new_dir->next_dir = next_dir;
      
      if(next_dir != NULL) {
        next_dir->prev_dir = new_dir;
      }
      curr_dir->next_dir = new_dir;

      return curr_dir->next_dir;

    }else if(next_dir == NULL || strcmp(dir_path, next_dir->dir_path) < 0) {
      new_dir = (dirNode*)malloc(sizeof(dirNode));
      dirNode_init(new_dir);

      new_dir->root_dir = dir_head->root_dir;
      strcpy(new_dir->backup_time,"");

      sprintf(new_dir->dir_name, "%s", GetFileName(dir_path));
      strcpy(new_dir->dir_path,dir_path);

      //부모의 주소를 적어넣어줍니다.
      strcpy(new_dir->parent_path,parentPath);

      new_dir->file_head = fileHead;

      new_dir->prev_dir = curr_dir;
      new_dir->next_dir = next_dir;
      
      if(next_dir != NULL) {
        next_dir->prev_dir = new_dir;
      }
      curr_dir->next_dir = new_dir;
      return curr_dir->next_dir;
    }
    curr_dir = next_dir;
  }
  return NULL;
}

//dirNode를 지워주는 함수입니다.
void dirNode_remove(dirNode *dir_node) {
  dirNode *next_dir = dir_node->next_dir;
  dirNode *prev_dir = dir_node->prev_dir;

  if(next_dir != NULL) {
    next_dir->prev_dir = dir_node->prev_dir;
    prev_dir->next_dir = dir_node->next_dir;
  } else {
    prev_dir->next_dir = NULL;
  }

  free(dir_node->subdir_head);
  free(dir_node->file_head);
  dir_node->subdir_head = NULL;
  dir_node->file_head = NULL;

  free(dir_node);
}

//원하는 dirNode를 경로를 통해서 찾는 함수입니다.
dirNode *dirNode_get(dirNode* dir_head, char* dir_path) {
  dirNode *curr_dir = dir_head->next_dir;
  
  while(curr_dir != NULL) {
    if(curr_dir != NULL && !strcmp(dir_path, curr_dir->dir_path)) {
      return curr_dir;
    }
    curr_dir = curr_dir->next_dir;
  }
  return NULL;
}



//깊이에 따라 다르게 출력하도록 도와주는 함수입니다.
void print_depth(int depth, int is_last_bit) {
  for(int i = 1; i <= depth; i++) {
    if(i == depth) {
      if((1 << depth) & is_last_bit) {
          printf("┗ ");
      } else {
          printf("┣ ");
      }
      break;
    }
    if((1 << i) & is_last_bit) {
      printf("  ");
    } else {
      printf("┃ ");
    }
  }
}


//트리구조를 출력하는 함수입니다.
void print_list(dirNode* dirList, int depth, int last_bit) {
  dirNode* curr_dir = dirList->subdir_head->next_dir;
  fileNode* curr_file = dirList->file_head->next_file;
  while(curr_dir != NULL && curr_file != NULL) {
    if(strcmp(curr_dir->dir_name, curr_file->file_name) < 0) {
      print_depth(depth, last_bit);
      printf("%s\n", curr_dir->dir_name);
      print_list(curr_dir, depth+1, last_bit);
      curr_dir = curr_dir->next_dir;
    } else {
      print_depth(depth, last_bit);
      printf("%s\n", curr_file->file_name);
      backupNode* curr_backup = curr_file->backup_head->next_backup;
      while(curr_backup != NULL) {
        print_depth(depth+1, last_bit);
        printf("[%s][%s] \n",curr_backup->action, curr_backup->backup_time);
        curr_backup = curr_backup->next_backup;
      }
      curr_file = curr_file->next_file;
    }
  }
  
  
  while(curr_dir != NULL) {
    last_bit |= (curr_dir->next_dir == NULL)?(1 << depth):0;
    
    print_depth(depth, last_bit);
    printf("%s\n", curr_dir->dir_name);
    print_list(curr_dir, depth+1, last_bit);
    curr_dir = curr_dir->next_dir;
  }
  
  while(curr_file != NULL) {
    last_bit |= (curr_file->next_file == NULL)?(1 << depth):0;

    print_depth(depth, last_bit);
    printf("%s\n", curr_file->file_name);
    backupNode* curr_backup = curr_file->backup_head->next_backup;
    while(curr_backup != NULL) {
      print_depth(depth+1, (
        last_bit | ((curr_backup->next_backup == NULL)?(1 << depth+1):0)
        ));
      printf("[%s][%s]\n",curr_backup->action, curr_backup->backup_time);
      curr_backup = curr_backup->next_backup;
    }
    curr_file = curr_file->next_file;
  }
}


//log를 읽어서 링크드리스트로 만들어주는 함수입니다.
int init_backup_list(int log_fd,char *getPath) {
  int len;
  char buf[BUF_MAX];

  char *backup_time, *origin_path, *backup_path;
  origin_path = (char*)malloc(sizeof(char)*PATHMAX);
  char*  need_path = (char*)malloc(sizeof(char)*PATHMAX);
  backup_time = (char*)malloc(sizeof(char)*PATHMAX);
  backup_dir_list = (dirNode*)malloc(sizeof(dirNode));
  dirNode_init(backup_dir_list);
  char line[PATHMAX];
  char action[20];
   
  while(len = read(log_fd, buf, BUF_MAX)) {
    char *ptr = strchr(buf, '\n');
    ptr[0] = '\0';
    lseek(log_fd, -(len-strlen(buf))+1, SEEK_CUR);

    
    sscanf(buf, "[%[^]]][%[^]]][%[^]]]",backup_time,action,origin_path);

    need_path = origin_path + strlen(getPath);
    
    backup_list_insert(backup_dir_list, backup_time, need_path, action);
    
  }
  printf("%s",getPath);
  print_list(backup_dir_list, 0, 0);

  return 0;
}


//큐도 초기화해서 만들어서 넣어줘야합니다.
//BFS를 사용하기 위해서 재귀적으로 반복하여 모니터링하는 함수입니다.
int backupRecursive(queue* Queue,char *path,dirNode *dir_head,char *backupLog,pid_t currentPid){
	struct stat statbuf,tempbuf;
  struct stat currentbuf,backupbuf;
	struct dirent **namelist;
  FILE* fp;
  int length,fd3,fd4;

  char *forDateBuffer  = (char *)malloc(sizeof(char)*BUF_MAX);
	char* tempPath = (char *)malloc(sizeof(char *)*PATHMAX);
  char *hash1 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash2 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *forWriteLog22 = (char *)malloc(sizeof(char)*PATHMAX*3);
  char *madePath = (char *)malloc(sizeof(char)*PATHMAX);
  char *gotDate  = (char *)malloc(sizeof(char)*15);
  char *buf = (char *)malloc(sizeof(char)*BUF_MAX);
  //새로운 파일링크드리스트를 생성해주는 것입니다.
	fileNode *headFile = (fileNode*)malloc(sizeof(fileNode));	
  fileNode_init(headFile);

	fileNode *forCheckFile;
	char *nextDir;
 
  //비교하기 위한 디렉토리 링크드리스트를 만들 위한 헤드를 만들어줍니다
  dirNode *compareDirHead = (dirNode *)malloc(sizeof(dirNode));
  dirNode_init(compareDirHead);

	int cnt;
	if(lstat(path,&statbuf)<0){
		fprintf(stderr,"ERROR: lstat cuase error\n");
    return 1;
	}
	if((cnt = scandir(path,&namelist,NULL,alphasort)) == -1){
		fprintf(stderr,"ERROR: cannot scan dir.\n");
		return 1;
	}
	for(int i = 0; i <cnt;i++){
		if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
    if(strstr(namelist[i]->d_name,".swp")!=NULL)continue;
    sprintf(tempPath,"%s/%s",path,namelist[i]->d_name);
    
	 if(lstat(tempPath,&tempbuf)<0){
		 fprintf(stderr,"lstat cause error.\n");
		 exit(1);
	 }
	 if(S_ISDIR(tempbuf.st_mode)){

		enqueue(Queue,tempPath);
    //삭제된 디렉토리가 있는 경우에는 노드에서 제거해줍니다.
    //그러기 위해서 비교하기 위한 리스트를 만들어줍니다.
    dirNode_insert_forR(compareDirHead,NULL,tempPath,path);
	 } //디렉토리인 경우에는 queue에 추가해줍니다.
	 if(S_ISREG(tempbuf.st_mode)){
    //파일인 경우에는 파일 링크드리스트에 추가해줍니다.
    //파일인 경우에는 create,modified에 대한 처리를 해주어야합니다.
    
    //아래는 create랑 같이 나오면 되는 것이다.
    if(dirNode_get(dir_head,path)==NULL){
      //directory도 없어서 해당 디렉토리 안의 파일들이 처음 존재하는 경우입니다.
      fileNode_insert_forD(headFile,tempPath);  
      strcpy(forDateBuffer,makeDate());
      fp = fopen(backupLog,"a+");
      fseek(fp,0,SEEK_END);
      sprintf(forWriteLog22,"[%s][create][%s]\n",forDateBuffer,tempPath);
      fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
      sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
      sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,GetFileName(tempPath),gotDate);
      fileNode* hiNode = fileNode_get(headFile,tempPath);
      strcpy(hiNode->made_path,madePath);
      fd3 = open(tempPath,O_RDONLY);
      fd4 = open(madePath,O_RDWR|O_CREAT,0777);
      //파일을 복구하여 줍니다.
      while((length = read(fd3,buf,BUF_MAX))>0){
        write(fd4,buf,length);
      }
      close(fd3);
      close(fd4);
      fclose(fp);
    }else{
      dirNode *currencyNode;
      if((currencyNode=dirNode_get(dir_head,path))!=NULL){
        if(fileNode_get(currencyNode->file_head,tempPath)==NULL){
          //여기서 create를 적어주는 것입니다.
          fileNode_insert_forD(headFile,tempPath);
          fileNode_insert_forD(currencyNode->file_head,tempPath);
          strcpy(forDateBuffer,makeDate());
          fp = fopen(backupLog,"a+");
          fseek(fp,0,SEEK_END);
          sprintf(forWriteLog22,"[%s][create][%s]\n",forDateBuffer,tempPath);
          fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
          sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
          sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,GetFileName(tempPath),gotDate);
          fileNode* hiNode = fileNode_get(currencyNode->file_head,tempPath);
          strcpy(hiNode->made_path,madePath);
          fd3 = open(tempPath,O_RDONLY);
          fd4 = open(madePath,O_RDWR|O_CREAT,0777);
          //파일을 복구하여 줍니다.
          while((length = read(fd3,buf,BUF_MAX))>0){
            write(fd4,buf,length);
          }
          close(fd3);
          close(fd4);
          fclose(fp);
        }else{
          //여기서 modified판단하는 것입니다.
          fileNode *currencyFile;
          if((currencyFile=fileNode_get(currencyNode->file_head,tempPath))!=NULL){
            stat(currencyFile->made_path,&backupbuf);
            stat(currencyFile->file_path,&currentbuf);
            if(backupbuf.st_size == currentbuf.st_size){
              if(currencyFile->firstMtime == currentbuf.st_mtime){
                md5(currencyFile->made_path,hash1);
                md5(currencyFile->file_path,hash2);
                if(strcmp(hash1,hash2)==0){
                  //두 파일이 변한 것 없이 아예 같은 경우이기 때문에 무시해주면 됩니다.
                }
                else{
                  //해쉬값이 다른 경우입니다.
                  fp = fopen(backupLog,"a+");
                  currencyFile->firstMtime = currentbuf.st_mtime;
                  strcpy(forDateBuffer,makeDate());
                  sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
                  sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currencyFile->file_name,gotDate);
                  strcpy(currencyFile->made_path,madePath);
                  //파일을 복구하여 줍니다.
                  fd3 = open(currencyFile->file_path,O_RDONLY);
                  fd4 = open(madePath,O_RDWR|O_CREAT,0777);
                  
                  while((length = read(fd3,buf,BUF_MAX))>0){
                    write(fd4,buf,length);
                  }
                  close(fd3);
                  close(fd4);

                  fseek(fp,0,SEEK_END);
                  sprintf(forWriteLog22,"[%s][modify][%s]\n",forDateBuffer,currencyFile->file_path);
                  fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
                  fclose(fp);
                }
              }else{
                //mtime이 다른 경우입니다.
                fp = fopen(backupLog,"a+");
                currencyFile->firstMtime = currentbuf.st_mtime;
                strcpy(forDateBuffer,makeDate());
                sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
                sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currencyFile->file_name,gotDate);
                strcpy(currencyFile->made_path,madePath);
                //파일을 복구하여 줍니다.
                fd3 = open(currencyFile->file_path,O_RDONLY);
                fd4 = open(madePath,O_RDWR|O_CREAT,0777);
                
                while((length = read(fd3,buf,BUF_MAX))>0){
                  write(fd4,buf,length);
                }
                close(fd3);
                close(fd4);

                fseek(fp,0,SEEK_END);
                sprintf(forWriteLog22,"[%s][modify][%s]\n",forDateBuffer,currencyFile->file_path);
                fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
                fclose(fp);
              }

            }else{
              //size가 다른 경우입니다.
              fp = fopen(backupLog,"a+");
              currencyFile->firstMtime = currentbuf.st_mtime;
              strcpy(forDateBuffer,makeDate());
              sscanf(forDateBuffer, "%4s-%2s-%2s %2s:%2s:%2s",gotDate, gotDate + 4, gotDate + 6,gotDate + 8, gotDate + 10, gotDate + 12);
              sprintf(madePath,"%s/%d/%s_%s",backupPATH,currentPid,currencyFile->file_name,gotDate);
              strcpy(currencyFile->made_path,madePath);
              //파일을 복구하여 줍니다.
              fd3 = open(currencyFile->file_path,O_RDONLY);
              fd4 = open(madePath,O_RDWR|O_CREAT,0777);
              
              while((length = read(fd3,buf,BUF_MAX))>0){
                write(fd4,buf,length);
              }
              close(fd3);
              close(fd4);

              fseek(fp,0,SEEK_END);
              sprintf(forWriteLog22,"[%s][modify][%s]\n",forDateBuffer,currencyFile->file_path);
              fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
              fclose(fp);
            }
          }
        }
      }
    }
	 }	
	}

  //이 함수는 이미 존재하는 경우에는 추가하지 않습니다.
	dirNode_insert_forR(dir_head,headFile,path,getParentPath(path));

  //parent가 같은 것 중에서 비교하면 remove를 찾을 수 있습니다.
  
  dirNode *currentDir;


  //해당디렉토리안의 파일이 지워진경우
  if((currentDir=dirNode_get(dir_head,path))!=NULL){
    fileNode *forFile;
    forFile = currentDir->file_head;
    forFile = forFile->next_file;
    while(forFile!=NULL){
      printf("list : %s\n",forFile->file_path);
      if(stat(forFile->file_path,&statbuf)<0){
        fp = fopen(backupLog,"a+");
        fseek(fp,0,SEEK_END);
        sprintf(forWriteLog22,"[%s][remove][%s]\n",makeDate(),forFile->file_path);
        fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
        fileNode *eraseNode = forFile;
        forFile = forFile->next_file;
        //노드를 링크드리스트에서 제거해줍니다.
        fileNode_remove(eraseNode);
        fclose(fp);
      }
      if(forFile==NULL){
        continue;
      }
      forFile = forFile->next_file;
    }
  }
  // 디렉토리 자체가 삭제된 경우
  currentDir = dir_head->next_dir;
  dirNode *compareDir;
  while(currentDir!=NULL){

    compareDir = compareDirHead->next_dir;
    while(compareDir!=NULL){
      if(strcmp(currentDir->parent_path,compareDir->parent_path)==0){
        if(dirNode_get(compareDirHead,currentDir->dir_path)==NULL){
          //remove된 것입니다.
          //dir자체가 지워진 상태입니다.
          fileNode *loopFile;

          loopFile=currentDir->file_head;
          
          loopFile = loopFile->next_file;
          while(loopFile!=NULL){
            fp = fopen(backupLog,"a+");
            fseek(fp,0,SEEK_END);
            sprintf(forWriteLog22,"[%s][remove][%s]\n",makeDate(),loopFile->file_path);
            fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
            fileNode *eraseNode = loopFile;
            loopFile = loopFile->next_file;
            //노드를 링크드리스트에서 제거해줍니다.
            fileNode_remove(eraseNode);
            fclose(fp);
          }
          dirNode *eraseDir = currentDir;
          currentDir = currentDir->next_dir;
          dirNode_remove(eraseDir);
          break;
        }
      }else if (currentDir->next_dir==NULL){
        if(stat(currentDir->dir_path,&statbuf)<0){
           fileNode *loopFile;

            loopFile=currentDir->file_head;
            
            loopFile = loopFile->next_file;
            while(loopFile!=NULL){
              fp = fopen(backupLog,"a+");
              fseek(fp,0,SEEK_END);
              sprintf(forWriteLog22,"[%s][remove][%s]\n",makeDate(),loopFile->file_path);
              fwrite(forWriteLog22,strlen(forWriteLog22),1,fp);
              fileNode *eraseNode = loopFile;
              loopFile = loopFile->next_file;
              //노드를 링크드리스트에서 제거해줍니다.
              fileNode_remove(eraseNode);
              fclose(fp);
            }
            dirNode *eraseDir = currentDir;
            currentDir = currentDir->next_dir;
            dirNode_remove(eraseDir);
            break;
        }
      }
      compareDir = compareDir->next_dir;
    }
    if(currentDir ==NULL){
      continue;
    }
    currentDir = currentDir->next_dir;
  }

	if(Queue->first ==NULL){
		return 0;
	}
  nextDir=dequeue(Queue);
	backupRecursive(Queue,nextDir,dir_head,backupLog,currentPid);

}

