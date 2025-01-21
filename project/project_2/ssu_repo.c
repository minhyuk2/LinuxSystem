#include "ssu_header.h"
//commit이 입력되었을 때 실행되는 함수입니다.
int CommitCommand(command_parameter *parameter){
  collectDir(useDirList->head,useStage->head);
	int cnt;
  int fd;
  int fd2;
  int fd3;
  int fd4;
  int fd5;
  int length;
  FILE *fp;
  FILE *fp2;
	char *dirname  = (char *)malloc(sizeof(char) * PATHMAX);
	char *newdirPath = (char *)malloc(sizeof(char) *PATHMAX);
  char *makedirPath = (char *)malloc(sizeof(char) *PATHMAX);
  char *haveMakeDirPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *haveMakeFilePath = (char *)malloc(sizeof(char)*PATH_MAX);
  char *buf = (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash1= (char *)malloc(sizeof(char)*BUF_MAX);
  char *hash2 = (char *)malloc(sizeof(char)*BUF_MAX);
  char *commitLog = (char *)malloc(sizeof(char)*BUF_MAX);
  char *lastestCommit = (char *)malloc(sizeof(char)*BUF_MAX);
  char *lastestCommitPath = (char *)malloc(sizeof(char)*PATH_MAX);
  char *forNewName = (char *)malloc(sizeof(char)*PATHMAX);
  char *relativePath = (char *)malloc(sizeof(char)*PATHMAX);
	dirList *dirHead = useDirList->head;
  stagingList *stageHead = useStage->head;
  stagingList *currStageHead;
  stagingList *newStageHead = (stagingList*)malloc(sizeof(stagingList));
	struct stat statbuf;
	struct stat statbuf2;
	struct dirent **namelist;
  changeFile *newChangeFile = (changeFile*)malloc(sizeof(changeFile));
  newChangeFile -> insertLine = 0;
  newChangeFile -> deleteLine = 0;
  newChangeFile -> fileCount = 0;

	if(useDirList->head->next ==NULL && useStage->head->next ==NULL){
		printf("Nothing to commit\n");
		return 0;
	}//스테이징에 올라와서 커밋할게 없는 경우에는 이렇게 작성하여 주는 것입니다.


  //인자로 입력받은 디렉토리 이름을 통해 새로운 경로를 만들어줍니다.
	strcpy(dirname,parameter->filename);
	sprintf(newdirPath,"%s/%s",repoPATH,dirname);
	
  //repoPATH에 lstat를 통해 접근할 수 있는지 확인하는 것입니다.
	if(lstat(repoPATH,&statbuf2)<0){
		fprintf(stderr, "open error %s\n",repoPATH);
		exit(1);
	}
  //scandir를 통해 repoPATH에 있는 파일들을 읽어옵니다.
	if((cnt= scandir(repoPATH, &namelist, NULL, alphasort))==-1){
		fprintf(stderr,"Scandir Error\n");
		exit(1);
	}


  for(int i = 0 ; i <cnt ;i++){
	  if(!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, "..")) continue;
    if(strcmp(dirname,namelist[i]->d_name)==0){
      printf("\"%s\" is already exist in repo\n",dirname);
      return 0;
    }
  }
  //같은 이름이 존재할 때에 대한 예외처리를 해줍니다.


  //커밋 로그를 작성하기 위한 접근을 확인합니다.
	if(lstat(commitPATH,&statbuf)<0){
		fprintf(stderr ,"lstat error %s\n",commitPATH);
		exit(1);
	}


	if(statbuf.st_size == 0){
	//아무것도 쓰여있지 않는 경우 비교하지 않기 위해서 따로 예외처리를 해주는 것이다.
  
    if(access(newdirPath,F_OK)<0){
       if(mkdir(newdirPath,0777)!=0){
		    	fprintf(stderr,"mkdir error for %s\n",newdirPath);
		    	exit(1);
		    }
    }
    //커밋로그를 적기위해 open해줍니다.
    if((fd = open(commitPATH,O_RDWR))<0){
		      fprintf(stderr,"open error %s\n",commitPATH);
		      exit(1);
	   }
    if((fp2 = fopen("temptemptempminhyuktemp.txt","wb+"))==NULL){
       fprintf(stderr,"open error \n");
		    exit(1);
    }
    //계층적으로 구성되어 있는 디렉토리 링크드 리스트를 반복시켜서 commit시켜줍니다.
    while(dirHead->next != NULL){
      dirHead = dirHead->next;
      currStageHead = dirHead->stageHead;

      while(currStageHead->next != NULL){

        currStageHead = currStageHead->next;
        sprintf(haveMakeDirPath,"%s%s",newdirPath,currStageHead->dirPath+strlen(exePATH));
        haveMakeDirPath[strlen(haveMakeDirPath)] = '\0';
	      if(access(haveMakeDirPath,F_OK)<0){
             while(mkdir(haveMakeDirPath, 0777)<0){
             //mkdir이 실패하였을 경우
                 accessDirCheck(haveMakeDirPath);
             }//파일을 생성할 수 있을 때까지 반복해준다.
         }
        sprintf(haveMakeFilePath,"%s%s",haveMakeDirPath,currStageHead->file_name);
        //새롭게 백업할 파일의 경로를 만들어서 open 시켜줍니다.
        if((fd2 = open(haveMakeFilePath,O_WRONLY|O_CREAT,0777))<0){
          fprintf(stderr,"open error for %s\n",haveMakeFilePath);
          exit(1);
        }
        //현재 파일의 정보를 읽기 위해서 열어줍니다.
        if((fd3 = open(currStageHead->path,O_RDONLY))<0){
          continue;
        }
        //내용을 그대로 적어서 백업시켜준다.
        while((length = read(fd3,buf,BUF_MAX))>0){
          write(fd2,buf,length);
        }
        
        sprintf(commitLog,"commit: \"%s\" -new file: \"%s\"\n",parameter->filename,currStageHead->path);
        write(fd,commitLog,strlen(commitLog)); 
        close(fd2);
        close(fd3);
        countFileLine(newChangeFile,currStageHead->path);
        newChangeFile->fileCount++;
        realpath_2_relative(currStageHead->path,relativePath);
        sprintf(commitLog,"new file: \"%s\"\n",relativePath);
        fputs(commitLog,fp2);
      }
    }	
    close(fd);

    //어띠로 커밋되었는지 적어주고
    //어떤 것들이 변하였는지 작성하여 줍니다.
    printf("commit to \"%s\"\n",parameter->filename);
    printf("%d files changed, %d insertions(+), %d deletions(-)\n",newChangeFile->fileCount,newChangeFile->insertLine,newChangeFile->deleteLine);
    fseek(fp2,0,SEEK_SET);
    //임시파일에서 문장들을 읽어와 출력시켜줍니다.
    while(fgets(buf,BUF_MAX,fp2)!=NULL){
      fputs(buf,stdout);
    }
    fclose(fp2);
 
  //임시파일을 닫은 후에 삭제시켜줍니다.
  if(remove("temptemptempminhyuktemp.txt")<0){
      printf("remove error\n");
  }
    //첫번 째 commit을 하였을 때입니다.
    return 0;
	}




  //commit log에 내용이 쓰여있어서 두 번째로 접근하는 경우입니다.
  //파일들의 변경내용을 저장하기 위한 변수를 선언합니다.

  int count=0;
  int offset=0;
  int optional =0;
  int startEnd[2];
  struct stat statbuf3;
  struct stat statbuf4;
  char *tempBuf = (char *)malloc(sizeof(char)*PATH_MAX);
  char *checkLine = (char *)malloc(sizeof(char)*PATH_MAX);
  //커밋 내용을 읽기 위해 fopen해줍니다.

  if((fp = fopen(commitPATH,"rb"))==NULL){
    fprintf(stderr,"fopen error for %s\n",commitPATH);
    exit(1);
  }

  //스테이지에 올라간 애들에 한해서 commit에 적힌 최신 버전을 가져옵니다.
  dirHead = useDirList->head;
  while(dirHead->next!= NULL){
    dirHead = dirHead->next;
    currStageHead = dirHead->stageHead;
    while(currStageHead->next != NULL){
      currStageHead = currStageHead->next;
      optional = 0;
      //한 줄씩 읽어서 스테이지에 올라간 애가 포함되어 있는지 확인합니다.
      while(fgets(buf,BUF_MAX,fp)!=NULL){
        if(strstr(buf,currStageHead->path)!=NULL)
          {
            char** substring = GetSubstring(buf,&count,"\"");
         
            strcpy(tempBuf,buf);
            optional = 1;
            if(strstr(substring[2],"removed")!=NULL){
              optional = 0;
            }
          }
      }

      if(optional==1){
          char** substring = GetSubstring(tempBuf,&count,"\"");
          sprintf(lastestCommitPath,"%s/%s%s",repoPATH,substring[1],substring[3]+strlen(exePATH));
          lastestCommitPath[strlen(lastestCommitPath)]='\0';
          char * filename = (char *)malloc(sizeof(char)*STRMAX);
          sprintf(lastestCommit,"%s/%s",repoPATH,substring[1]);
          lastestCommit[strlen(lastestCommit)] = '\0';
          addStagingList(newStageHead,lastestCommitPath,lastestCommit);
      }
      fseek(fp,0,SEEK_SET);
    }
  }
  fclose(fp);
  //읽기모드로 열었던 파일포인터를 닫아줍니다.


  //commit내용에 추가하기 위해 fopen해줍니다.
  if((fp = fopen(commitPATH,"ab"))==NULL){
    fprintf(stderr,"fopen error for %s\n",commitPATH);
    exit(1);
  }
  //commit된 기록들을 한 번에 출력하기 위해 임시파일을 생성합니다.
  if((fp2 = fopen("temptemptempminhyuktemp.txt","wb+"))==NULL){
       fprintf(stderr,"open error \n");
		    exit(1);
  }
  
  //이제 삭제된 것이나 같은 게 있는지 검사하는 부분입니다.
  dirHead = useDirList->head;
  stagingList *loopHead;
  int optioning = 0;

  //BFS로 트리 구조에서 레벨링을 통해 정렬된 디렉토리 링크드 리스트에서 해당 디렉토리의 파일들을 가지고 new file, removed, modified를 확인합니다.
  while(dirHead->next != NULL){
    
    //파일이 하나라도 변하지 않는 경우에는 백업 디렉토리를 생성하지 않기 위해 설정해둔 것입니다.
    if(newChangeFile->fileCount !=0) {
        if(access(newdirPath,F_OK)<0){
       if(mkdir(newdirPath,0777)!=0){
		    	fprintf(stderr,"mkdir error for %s\n",newdirPath);
		    	exit(1);
		    }
    }
    }


    dirHead = dirHead->next;
    loopHead = newStageHead;
    currStageHead = dirHead->stageHead;
     //최신의 백업파일을 돌면서 얻는 파일들에 대해서 반복문을 수행합니다.
     while(loopHead->next!= NULL){
        loopHead = loopHead->next;
        currStageHead = dirHead->stageHead;
        optioning = 0;

        while(currStageHead->next != NULL){
          currStageHead = currStageHead->next;
          sprintf(forNewName,"%s",currStageHead->path+strlen(exePATH)+1);
          forNewName[strlen(forNewName)]='\0';
          //작업중인 경로를 제외한 것의 문자열을 얻습니다.
          //그 문자열과 백업파일의 문자열의 백업디렉토리를 제외한 부분의 문자열을 서로 비교합니다.

           if(strcmp(loopHead->file_name+1,forNewName)==0){
             
              if(stat(loopHead->path,&statbuf3)<0){
                 fprintf(stderr,"stat error for %s\n",loopHead->path);
                 exit(1);
              }

              if(stat(currStageHead->path,&statbuf4)<0){
                //삭제된 애가 스테이지에 올라와 있을 수 있기 때문에 이에 대한 예외처리를 함으로써 removed를 나타낼 수 있습니다.
                optioning =1;
                break;
              }
              //stat함수를 사용하여 파일의 사이즈를 먼저 비교합니다.
              if(statbuf3.st_size == statbuf4.st_size){
                md5(currStageHead->path,hash1);
                md5(loopHead->path,hash2);
                //md5로 hash하였는데 hash값까지 같은 경우입니다.
                if(strcmp(hash1,hash2)==0){
                  
                  break;
                }

                else{
                  //md5값이 다른 경우
                  //한 줄씩 비교하여 삭제된 것과 추가된 것을 설정한다.
                  compareFile(loopHead->path,currStageHead->path,newChangeFile);
                  newChangeFile->fileCount++;
                  if(newChangeFile->fileCount !=0) {
                          if(access(newdirPath,F_OK)<0){
                            if(mkdir(newdirPath,0777)!=0){
                            fprintf(stderr,"mkdir error for %s\n",newdirPath);
                            exit(1);
                           }
                       }
                   }
                  //백업 디렉토리 안에 백업파일을 만들어줍니다.
                  sprintf(haveMakeDirPath,"%s%s",newdirPath,currStageHead->dirPath+strlen(exePATH));
                  haveMakeDirPath[strlen(haveMakeDirPath)] = '\0';
                      if(access(haveMakeDirPath,F_OK)<0){
                           while(mkdir(haveMakeDirPath, 0777)<0){
                              //mkdir이 실패하였을 경우
                                  accessDirCheck(haveMakeDirPath);
                              }//파일을 생성할 수 있을 때까지 반복해준다.
                          }
                  sprintf(haveMakeFilePath,"%s%s",haveMakeDirPath,currStageHead->file_name);

                  //백업파일을 적어주기 위해서 생성해줍니다.

                  if((fd4 = open(haveMakeFilePath,O_WRONLY|O_CREAT,0777))<0){
                      fprintf(stderr,"open error for %s\n",haveMakeFilePath);
                      exit(1);
                    }
                  //기존의 파일을 열어서 읽어줍니다.
                  if((fd5 = open(currStageHead->path,O_RDONLY))<0){
                      continue;
                    }

                  while((length = read(fd5,buf,BUF_MAX))>0){
                      write(fd4,buf,length);
                  }     
                  close(fd4);
                  close(fd5);
                  //commit 로그를 작성해줍니다.
                  sprintf(commitLog,"commit: \"%s\" - modified: \"%s\"\n",parameter->filename,currStageHead->path);
                  fputs(commitLog,fp);
                  //상대경로로 변환하여 출력해주기 위해서 임시파일에 해당내용을 작성하여 줍니다.
                  realpath_2_relative(currStageHead->path,relativePath);
                  sprintf(commitLog,"modified: \"%s\"\n",relativePath);
                  fputs(commitLog,fp2);
                
                  break;
                }
              }
              else{
                //파일의 길이가 다른 경우
                //한 줄씩 비교하여 삭제된 것과 추가된 것을 설정한다.
                  compareFile(loopHead->path,currStageHead->path,newChangeFile);
                  newChangeFile->fileCount++;
                     if(newChangeFile->fileCount !=0) {
                          if(access(newdirPath,F_OK)<0){
                            if(mkdir(newdirPath,0777)!=0){
                            fprintf(stderr,"mkdir error for %s\n",newdirPath);
                            exit(1);
                           }
                       }
                   }
                  sprintf(haveMakeDirPath,"%s%s",newdirPath,currStageHead->dirPath+strlen(exePATH));
                  haveMakeDirPath[strlen(haveMakeDirPath)] = '\0';

                      if(access(haveMakeDirPath,F_OK)<0){
                           while(mkdir(haveMakeDirPath, 0777)<0){
                              //mkdir이 실패하였을 경우
                                  accessDirCheck(haveMakeDirPath);
                              }//파일을 생성할 수 있을 때까지 반복해준다.
                          }
                  sprintf(haveMakeFilePath,"%s%s",haveMakeDirPath,currStageHead->file_name);

                  if((fd4 = open(haveMakeFilePath,O_WRONLY|O_CREAT,0777))<0){
                      fprintf(stderr,"open error for %s\n",haveMakeFilePath);
                      exit(1);
                    }
                  if((fd5 = open(currStageHead->path,O_RDONLY))<0){
                      continue;
                    }
                    while((length = read(fd5,buf,BUF_MAX))>0){
                      write(fd4,buf,length);
                  }     
                  close(fd4);
                  close(fd5);

                  sprintf(commitLog,"commit: \"%s\" - modified: \"%s\"\n",parameter->filename,currStageHead->path);
                  fputs(commitLog,fp);
                  realpath_2_relative(currStageHead->path,relativePath);
                  sprintf(commitLog,"modified: \"%s\"\n",relativePath);
                  fputs(commitLog,fp2);
                  break;
                
              }
           }
      }
      //백업이 되어있고 스테이징에 올라와 있지만 작업중인 디렉토리 안에서 삭제된 것에 대해서 removed로 적힐 수 있게 설정해줍니다.
      if(optioning == 1){
      
      //삭제되어서 존재하지 않는 파일인지 다시 한 번 확인해줍니다.
      strcpy(checkLine,currStageHead->path);
      if(access(checkLine,F_OK)<0){
          sprintf(commitLog,"commit: \"%s\" - removed: \"%s\"\n",parameter->filename,checkLine);
          fputs(commitLog,fp);
          realpath_2_relative(checkLine,relativePath);
          sprintf(commitLog,"removed: \"%s\"\n",relativePath);
          fputs(commitLog,fp2);
          newChangeFile->fileCount++;
      }
      }
      optioning=0;
  }  

    //다시 반대의 과정으로 반복문을 돌리기 위해서 초기값을 설정합니다.
    currStageHead = dirHead->stageHead;
    loopHead = newStageHead;

    while(currStageHead->next != NULL){
      int option = 0;
      currStageHead = currStageHead->next;
      sprintf(forNewName,"%s",currStageHead->path+strlen(exePATH)+1);
      forNewName[strlen(forNewName)]='\0';
      loopHead = newStageHead;
      while(loopHead->next!= NULL){
          loopHead = loopHead->next;
          if(strcmp(loopHead->file_name+1,forNewName)==0){
              option = 1;
          }
          
      }

      if(option == 0){
        //여기에 있으면 추가된 것
        //추가된 것을 확인 할 때는 access되는지를 통해 확인해보면 된다.
        if(access(currStageHead->path,F_OK)<0){
          continue;
        }
         newChangeFile->fileCount++;
          if(newChangeFile->fileCount !=0) {
                  if(access(newdirPath,F_OK)<0){
                    if(mkdir(newdirPath,0777)!=0){
                      fprintf(stderr,"mkdir error for %s\n",newdirPath);
                      exit(1);
                  }
              }
          }
        sprintf(haveMakeDirPath,"%s%s",newdirPath,currStageHead->dirPath+strlen(exePATH));
        haveMakeDirPath[strlen(haveMakeDirPath)] = '\0';
                      if(access(haveMakeDirPath,F_OK)<0){
                           while(mkdir(haveMakeDirPath, 0777)<0){
                              //mkdir이 실패하였을 경우
                                  accessDirCheck(haveMakeDirPath);
                              }//파일을 생성할 수 있을 때까지 반복해준다.
                          }
          sprintf(haveMakeFilePath,"%s%s",haveMakeDirPath,currStageHead->file_name);
          printf("%s\n",haveMakeFilePath);
                  if((fd4 = open(haveMakeFilePath,O_WRONLY|O_CREAT,0777))<0){
                      fprintf(stderr,"open error for %s\n",haveMakeFilePath);
                      exit(1);
                    }
                  if((fd5 = open(currStageHead->path,O_RDONLY))<0){
                      continue;
                    }
                    while((length = read(fd5,buf,BUF_MAX))>0){
                      write(fd4,buf,length);
                  }     
                  close(fd4);
                  close(fd5);
          //위의 과정들과 동일하게 commit로그에 작성하고 출력될 내용을 임시파일에 저장합니다.
          sprintf(commitLog,"commit: \"%s\" - new file: \"%s\"\n",parameter->filename,currStageHead->path);
          commitLog[strlen(commitLog)]='\0';
          fputs(commitLog,fp);
          realpath_2_relative(currStageHead->path,relativePath);
          sprintf(commitLog,"new file: \"%s\"\n",relativePath);
          fputs(commitLog,fp2);
          //파일의 줄의 개수를 적어서 포함시켜줍니다.
          countFileLine(newChangeFile,currStageHead->path);

      }
    }
    
  }




  //변한 파일이 없을 경우에는 아래와 같이 출력해줍니다.
  if(newChangeFile->fileCount == 0){
    printf("Nothing to Commit\n");
  }
  else{
    //어띠로 커밋되었는지 적어주고
    //어떤 것들이 변하였는지 작성하여 줍니다.
    printf("commit to \"%s\"\n",parameter->filename);
    printf("%d files changed, %d insertions(+), %d deletions(-)\n",newChangeFile->fileCount,newChangeFile->insertLine,newChangeFile->deleteLine);
    fseek(fp2,0,SEEK_SET);
    //임시파일에서 문장들을 읽어와 출력시켜줍니다.
    while(fgets(buf,BUF_MAX,fp2)!=NULL){
      fputs(buf,stdout);
    }
  }
  fclose(fp);
  close(fd);
  fclose(fp2);
  //임시파일을 닫은 후에 삭제시켜줍니다.
  if(remove("temptemptempminhyuktemp.txt")<0){
      printf("remove error\n");
  }
  return 0;
}


//add명령어를 입력하였을 때 실행되는 함수입니다.
int AddCommand(command_parameter *parameter) {
	struct stat statbuf;

  char *tmpPath = (char *)malloc(sizeof(char *) * PATHMAX);
  char *originPath = (char *)malloc(sizeof(char *) * PATHMAX);
  char *newBackupPath = (char *)malloc(sizeof(char *) * PATHMAX);
  char **backupPathList = NULL;
  char *logString = (char *)malloc(sizeof(char)*(PATHMAX*2 + 6));
  char *relativePath = (char *)malloc(sizeof(char) *PATHMAX);

  int fd;

  strcpy(originPath, parameter->filename);
  //상대경로로 바꿔줍니다.
  realpath_2_relative(originPath,relativePath);
  
  if (lstat(originPath, &statbuf) < 0) {
    fprintf(stderr, "ERROR: lstat error for %s\n", originPath);
    return 1;
  }//path에 접근할 수 있는지 확인합니다.

  if(!S_ISREG(statbuf.st_mode) && !S_ISDIR(statbuf.st_mode)) {
    fprintf(stderr, "ERROR: %s is not directory or regular file\n", originPath);
    return 1;
  }
  //파일인 경우와 디렉토리인 경우에 다르게 처리해주면 됩니다.
  
  if(S_ISREG(statbuf.st_mode)){
    //파일인 경우에는 이제 dir랑 file이랑 다 돌면서 찾으면 되는 것입니다.
    if(findingStagingNode(useStage->head,originPath) == 1){
      printf("\"%s\" already exist in staging area\n",relativePath);
      return 1;
    }//찾는게 있으면 추가할 필요가 없기 때문에 더이상 찾지 않는 것이다.
    if(searchingNode(useDirList->head,originPath,1)==1){
      printf("\"%s\" already exist in staging area\n",relativePath);
      return 1;
    }
    //있는지 확인하는 경우다.
  }

  if(S_ISDIR(statbuf.st_mode)){
    //디렉토리인 경우입니다.
    if(searchingNode(useDirList->head,originPath,0)==1){
      printf("\"%s\" already exist in staging area\n",relativePath);
      return 1;
    }
  }//없는 경우에는 다 적어주면 되는 것이다.

  //staging log를 열어서 거기에 적어주면 된다.
  if((fd = open(stagingPATH, O_APPEND|O_RDWR))<0){
    fprintf(stderr,"file open error\n");
    exit(1);
  }
  //add를 실행한 내용을 staging로그에 작성해줍니다.
  sprintf(logString,"add \"%s\"\n",originPath);
  write(fd,logString,strlen(logString));

  //add한 것에 대한 결과물을 화면상에 출력해줍니다. 
  printf("add \"%s\"\n",relativePath);
  return 0;
}

//remove명령어를 시행했을 경우입니다.
int RemoveCommand(command_parameter *parameter){

  struct stat statbuf;

  char *tmpPath = (char *)malloc(sizeof(char *) * PATHMAX);
  char *originPath = (char *)malloc(sizeof(char *) * PATHMAX);
  char *newBackupPath = (char *)malloc(sizeof(char *) * PATHMAX);
  char **backupPathList = NULL;
  char *logString = (char *)malloc(sizeof(char)*(PATHMAX*2 + 6));
  char *relativePath = (char *)malloc(sizeof(char) *PATHMAX);
  int fd;
  strcpy(originPath, parameter->filename);

  realpath_2_relative(originPath,relativePath);

  if (lstat(originPath, &statbuf) < 0) {
    //삭제된 애를 선제적으로 명령어가 입력받을 때 처리를 해주기 때문에 그게 그대로 넘어오는 경우에 대한 에러처리를 해주어야 합니다.
    return 1;
  }
 
  //파일이나 디렉토리가 아닌 경우에 대한 예외처리를 해주는 것입니다.
  if(!S_ISREG(statbuf.st_mode) && !S_ISDIR(statbuf.st_mode)) {
    fprintf(stderr, "ERROR: %s is not directory or regular file\n", originPath);
    return 1;
  }

  //이미 삭제되어 있는지 확인하는 과정입니다.
  if(S_ISDIR(statbuf.st_mode)){
    //일단 dir에 대한 예외처리를 해주는 것입니다.
    if(searchingNode(removeList->head,originPath,0)==1){  
      printf("\"%s\" already removed from staging area\n",relativePath);
      return 0;
    }
  }

  if(S_ISREG(statbuf.st_mode)){
    //파일인 경우에 대한 처리를 해주는 것입니다.
    if(findingStagingNode(removeFileList->head,originPath)==1){
      printf("\"%s\" already removed from staging area\n",relativePath);
      return 0;
    }

    if(searchingNode(removeList->head,originPath,1)==1){
      printf("\"%s\" already removed from staging area\n",relativePath);
      return 0;
    }
  }
  //삭제되어 있지 않은 것들을 staging로그를 열어서 작성해주는 것입니다.
 if((fd = open(stagingPATH, O_APPEND|O_RDWR))<0){
       fprintf(stderr,"file open error\n");
        exit(1);
        }

  sprintf(logString,"remove \"%s\"\n",originPath);
  write(fd,logString,strlen(logString));
  printf("remove \"%s\"\n",relativePath);

}
//help와 다른 명령어가 입력되었을 경우에 대한 처리입니다.
int HelpCommand(command_parameter *parameter){
  char *input = (char *)malloc(sizeof(char)*STRMAX);
  strcpy(input,parameter->filename);
  if(strcmp("add",parameter->filename)==0){
    printAdd();
    return 0;
  }else if(strcmp("remove",parameter->filename)==0){
    printRemove();
    return 0;
  }else if(strcmp("status",parameter->filename)==0){
    printStatus();
    return 0;
  }else if(strcmp("commit",parameter->filename)==0){
    printCommit();
    return 0;
  }else if(strcmp("revert",parameter->filename)==0){
    printRevert();
    return 0;
  }else if(strcmp("log",parameter->filename)==0){
    printLog();
    return 0;
  }else if(strcmp("help",parameter->filename)==0){
    printHelp();
    return 0;
  }else if(strcmp("exit",parameter->filename)==0){
    printExit();
    return 0;
  }
  else{
    printAll();
    return 0;
  }
  return 0;
}



//fork()되었을 때 어떤 함수를 실행할지 설정해주는 함수입니다.
void CommandFun(char **arglist) {
  int (*commandFun)(command_parameter * parameter);
  //함수 포인터를 생성하는 것이다.
	command_parameter parameter={
    arglist[0], arglist[1], arglist[2], arglist[3]
  };

  if(!strcmp(parameter.command, commanddata[0])) {
    commandFun = AddCommand;
  }
   else if(!strcmp(parameter.command, commanddata[1])) {
    commandFun = RemoveCommand;
   }
//   } else if(!strcmp(parameter.command, commanddata[2])) {
//     commandFun = StatusCommand;
   else if(!strcmp(parameter.command, commanddata[3])) {
    commandFun = CommitCommand;
   }
   else if(!strcmp(parameter.command, commanddata[6])) {
    commandFun = HelpCommand;
  }
  // else if(!strcmp(parameter.command, commanddata[5])) {
  //   commandFun = LogCommand;
  // }
  if(commandFun(&parameter) != 0) {
    exit(1);
  }//이때 함수 실행시키는 것입니다.
}

//fork()시켜주는 함수입니다.
void CommandExec(command_parameter parameter) {
  pid_t pid;
  //parameter값을 설정해주는 것입니다.
  parameter.argv[0] = "command";
  parameter.argv[1] = parameter.command;
  parameter.argv[2] = parameter.filename;
  parameter.argv[3] = parameter.tmpname;
  parameter.argv[4] = (char *)0;
  //마지막이 항상 null로 끝나야하기에 마지막에 넣어주는 것입니다.
  if((pid = fork()) < 0) {
    fprintf(stderr, "ERROR: fork error\n");
    exit(1);
  } else if(pid == 0) {
    execv(exeNAME, parameter.argv);
    exit(0);
  } else {
    pid = wait(NULL);
  }
} //command로 적어두었던 것들을 exec함수로 실행하는 것입니다.


//helpExec으로 잘못된 명령어가 들어왔을 때 help명령어를 수행하기 위해서 따로 fork()해주는 함수입니다.
void HelpExec() {
  pid_t pid;

  if((pid = fork()) < 0) {
    fprintf(stderr, "ERROR: fork error\n");
    exit(1);
  } else if(pid == 0) {
    execl(exeNAME, "help", (char *)0);
    exit(0);
  } else {
    pid = wait(NULL);
  }
} //help에 대한 프로세스를 만들어주는 함수입니다.

//parameter를 초기화해주는 함수입니다.
void ParameterInit(command_parameter *parameter) {
  parameter->command = (char *)malloc(sizeof(char *) * PATHMAX);
  parameter->filename = (char *)malloc(sizeof(char *) * PATHMAX);
  parameter->tmpname = (char *)malloc(sizeof(char *) * PATHMAX);
} 

//인자로 받은 것에 대한 처리를 진행해주는 함수입니다.
int ParameterProcessing(int argcnt, char **arglist, int command, command_parameter *parameter) {
    struct stat buf;
    struct stat statbuf;
    int fd;
    char *logString = (char *)malloc(sizeof(char)*(PATHMAX+8));
    char *relativePath = (char *)malloc(sizeof(char) *PATHMAX);
    switch(command){
        case CMD_ADD: {
          //ADD의 인자로 넣어줄 것에 대한 것들의 예외처리를 해주는 것입니다.
          if (argcnt < 2) {
			        	fprintf(stderr,"ERROR: <PATH> is not include\n");
                printAdd();
			      	return -1;
	      		}
          if(realpath(arglist[1],parameter->filename)==NULL) {
                realpath_2_relative(arglist[1], parameter->filename);
                printf("ERROR: \'%s\' is wrong path\n",parameter->filename+2);
                return -1;
           }
           if (lstat(parameter->filename, &buf) < 0) {
                fprintf(stderr, "ERROR: lstat error for %s\n", parameter->filename);
                return -1;
            }
          if(strstr(parameter->filename,exePATH)==NULL){
                printf("ERROR: \'%s\' is wrong path\n",parameter->filename);
                return -1;
          }
          
            break;
        }
        case CMD_REM : {
            //remove명령어가 입력되었을 때 해줄 수 있는 예외처리입니다.
            if (argcnt < 2) {
				        fprintf(stderr,"ERROR: <PATH> is not include\n");
                printRemove();
			        	return -1;
		        	}
           if(realpath(arglist[1],parameter->filename)==NULL) {
              char *makeReal = (char *)malloc(sizeof(char)*PATH_MAX);
                if(arglist[1][0] != '/' ){
                realpath_2_relative(arglist[1],relativePath);
                sprintf(makeReal,"%s%s",exePATH,relativePath+1);
                makeReal[strlen(makeReal)]= '\0';
                }else{
                  strcpy(makeReal,arglist[1]);
                }
                
                if(findingStagingNode(useStage->head,makeReal)==1){
                  //파일은 삭제되었는데 staging에는 적혀 있는 경우에 대한 예외처리입니다.
                  deleteStagingList(useStage->head, makeReal);
                  if((fd = open(stagingPATH, O_APPEND|O_RDWR))<0){
                    fprintf(stderr,"file open error\n");
                    exit(1);
                    }
                  sprintf(logString,"remove \"%s\"\n",makeReal);
                  write(fd,logString,strlen(logString));
                  printf("remove \"%s\"\n",relativePath);
                  return 0;
                }
                if(searchingNode(useDirList->head,makeReal,0)==1){
                  //디렉토리안의 파일이었던 경우에 삭제된 애의 추적을 끊는다고 할 때입니다.
                  deleteNodeInDir(useDirList->head,makeReal);
                   if((fd = open(stagingPATH, O_APPEND|O_RDWR))<0){
                    fprintf(stderr,"file open error\n");
                    exit(1);
                    }
                  sprintf(logString,"remove \"%s\"\n",makeReal);
                  write(fd,logString,strlen(logString));
                  printf("remove \"%s\"\n",relativePath);
                  return 0;
                }
                if(searchingNode(useDirList->head,makeReal,1)==1){
                  //디렉토리인 경우에 대한 예외처리입니다.
                 deleteDir(useDirList->head,makeReal,0);
                   if((fd = open(stagingPATH, O_APPEND|O_RDWR))<0){
                    fprintf(stderr,"file open error\n");
                    exit(1);
                    }
                  sprintf(logString,"remove \"%s\"\n",makeReal);
                  write(fd,logString,strlen(logString));
                  printf("remove \"%s\"\n",relativePath);
                  return 0;
                }
                realpath_2_relative(arglist[1], parameter->filename);
                printf("ERROR: \'%s\' is wrong path\n",parameter->filename);
                return -1;
            }
            //다시 한 번 확인 했을 경우에 대한 예외처리입니다.
            if (lstat(parameter->filename, &buf) < 0) {
                fprintf(stderr, "ERROR: lstat error for %s\n", parameter->filename);
                return -1;
            }

            break;
        }
        case CMD_STA : {
            break;
        }
        case CMD_COM : {
            if (argcnt < 2) {
			        	fprintf(stderr,"ERROR: <NAME> is not include\n");
                printCommit();
				        return -1;
		        }
            char *newName = (char *)malloc(sizeof(char)*NAMEMAX);
            char *tempName = (char *)malloc(sizeof(char)*NAMEMAX);
            strcpy(newName,arglist[1]);
            strcpy(parameter->filename,newName);

            break;
        }
        case CMD_REV : {
            if (argcnt < 2) {
				        fprintf(stderr,"ERROR: <NAME> is not include\n");
                printRevert();
				        return -1;
		        	}
            break;
        }
        case CMD_LOG :{
            break;
        }
        case CMD_HELP : {
          strcpy(parameter->filename,arglist[1]);
          break;
        }
    }
} //인자의 개수와 매개변수로 들어온 파일에 대한 예외처리를 미리 해주었다.


//계속 출력되어 나오는 프롬프트를 설정하는 함수이다.
int Prompt() {
  char input[STRMAX];
	int argcnt = 0;
	char **arglist = NULL;
  int command;
	command_parameter parameter={(char *)0, (char *)0, (char *)0, 0};

  while(true) {
    printf("20202920> ");
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = '\0';
    //계속해서 문자열을 입력받는 것이다.
		if((arglist = GetSubstring(input, &argcnt, " \t")) == NULL) {
      continue;
    }
    //"로 시작하는 경우에 대한 예외처리를 진행해주는 것이다.
    if(argcnt > 1){
    if(arglist[1][0]=='\"'){
      int counting =0;
      char *newPathing = (char *)malloc(sizeof(char)*PATH_MAX);
      for(int i=0; i<strlen(input);i++){
        counting++;
        if(input[i] =='\"'){
          break;
        }
      }
      strcpy(newPathing,input+counting);
      newPathing[strlen(newPathing)-1] = '\0'; 
      strcpy(arglist[1],newPathing);
      }
    }
    if(argcnt == 0) continue;

		if(!strcmp(arglist[0], commanddata[0])) {
			command = CMD_ADD;
		} else if(!strcmp(arglist[0], commanddata[1])) {
			command = CMD_REM;
		}	else if(!strcmp(arglist[0], commanddata[2])) {
			command = CMD_STA;
		}	else if(!strcmp(arglist[0], commanddata[3])) {
			command = CMD_COM;
		}	else if(!strcmp(arglist[0], commanddata[4])) {
			command = CMD_REV;
		}	else if(!strcmp(arglist[0], commanddata[5])) {
			command = CMD_LOG;
		}	else if(!strcmp(arglist[0], commanddata[6])) {
			command = CMD_HELP;
		}	else if(!strcmp(arglist[0], commanddata[7])) {
			command = CMD_EXIT;
             exit(0);
            //exit이 들어오면 프로그램 종료
		}	else {
			command = NOT_CMD;
		}
        
    if(command & (CMD_ADD | CMD_REM | CMD_STA | CMD_COM | CMD_REV| CMD_LOG | CMD_HELP)) {
		  ParameterInit(&parameter);
      parameter.command = arglist[0];
      //command에 어떤 command인지 넣어준다.
      if(ParameterProcessing(argcnt, arglist, command, &parameter) == -1) {
        continue;
      } //-1이 리턴되면 설정된 일부분에서 오류가 발생한 것이기 때문에 -1을 리턴한다.

    CommandExec(parameter);

    }else if(command == NOT_CMD) {
      HelpExec();
    }
  }
}

//처음 프로그램이 실행될 때 기본 설정을 해주는 함수다.ㄴ
void Init() {
  int fd;
  int fd2;
  int count;
  char *buf =(char *)malloc(sizeof(char *) * STRMAX);
  char *line =(char *)malloc(sizeof(char) * STRMAX);
  char *repoPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *commitPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *stagingPath = (char *)malloc(sizeof(char)*PATHMAX);
  int len;
  struct stat statbuf;
  struct stat statbuf2;
  //여기에 queue를 하나 만들어서 넘겨주면 된다.
  queue* Queue = (queue*)malloc(sizeof(queue));
	Queue->first = NULL;
	Queue->last = NULL;
  stagingList *head = (stagingList *)malloc(sizeof(stagingList));
  stagingList *removeStageHead = (stagingList *)malloc(sizeof(stagingList));

  //처음에 헤드를 만들어준다.

  
  dirList *dirListHead = (dirList*)malloc(sizeof(dirList));
  dirList *removeDirHead = (dirList *)malloc(sizeof(dirList));

  useDirList = (dirList*)malloc(sizeof(dirList));
  useDirList -> head = dirListHead;
  useDirList ->head ->next = NULL;

  //초기화 해주는 것이다.
  removeList = (dirList *)malloc(sizeof(dirList));
  removeList->head = removeDirHead;
  removeList->head ->next = NULL;


  useStage = (stagingList*)malloc(sizeof(stagingList));
  useStage -> head = head;
  useStage ->head -> next = NULL;

  removeFileList = (stagingList*)malloc(sizeof(stagingList));
  removeFileList -> head  =  removeStageHead;
  removeFileList ->head ->next = NULL;


  getcwd(exePATH, PATHMAX);
  //현재 실행중인 파일의 경로를 알려준다.
  sprintf(homePATH, "%s", getenv("HOME"));
  //home 경로를 알려준다.
 
  sprintf(repoPath,"%s/%s",exePATH,".repo");
  sprintf(repoPATH,"%s",repoPath);


  if(access(repoPath, F_OK)!=0){
        mkdir(".repo", 0777);
  }//.repo 디렉토리가 없으면 생성해준다.

  sprintf(commitPath,"%s/%s",repoPath,".commit.log");
  sprintf(stagingPath,"%s/%s",repoPath,".staging.log");

  sprintf(stagingPATH,"%s",stagingPath);
  sprintf(commitPATH,"%s",commitPath);


  if((fd = open(commitPath,O_RDWR|O_CREAT,0777))<0){
    fprintf(stderr,"error cause\n");
	  exit(1);
  }
  
  if((fd2 = open(stagingPath,O_RDWR|O_CREAT,0777))<0){
    fprintf(stderr,"error cause\n");
		exit(1);
  }
  //commit.log파일과 staging.log파일을 생성해준다.

  //이미 생성되어있는 경우의 내용들을 읽어서 링크드리스트로 만들어준다.
  if(lstat(stagingPath,&statbuf)<0){
    fprintf(stderr,"lstat error\n");
    exit(1);
  }
  while((len = read(fd2,buf,statbuf.st_size))>0){
    char** substring = GetSubstring(buf,&count,"\n");
      //이렇게 하면 개행문자를 기준으로 문장을 하나씩 잘라서  substring에 넣어주는 것이다.
      //substring에 하나씩 넣은 다음에 이걸 이제 또 하나씩 자르면 된다.
    for(int i = 0 ; i < count; i++){
      int count2;

      if(substring[i][0] == 'a'){
        //add로 시작할 때는 첫글자가 add일테니까 처리해주는 것입니다.
        char** substring2 = GetSubstring(substring[i],&count2,"\"");
        //경로상에 "가 나올 수 없기 때문에 무조건 경로는 배열의 두 번째일 것이다.
        //그게 파일인지 디렉토리인지 구분에서 파일이면 그냥 넣고 디렉토리인 경우에 안에 있는 것들을 분석해서 넣되, 이미 들어있는게 아닌지는 구분해줘야 한다.
        //만약에 사라진 파일이면 이게 안되는디 이게 중간에 파일이 사라져버리면 오류가 생기니까    
        char *currentPath = malloc(sizeof(char)*PATHMAX);
        strcpy(currentPath,substring2[1]);
        if(lstat(currentPath,&statbuf2)<0){
          //삭제된 애들도 추가해주는 것이다.
          //근데  한 번만 추가되게 해두어야한다.
          char *currentDir = malloc(sizeof(char)*PATHMAX);
          int idx;
          int option=0;
          stagingList *currNode = useStage->head;
          for( idx = strlen(currentPath)-1; currentPath[idx] != '/'; idx--);
          strncpy(currentDir,currentPath,idx);
          currentDir[idx] = '\0';
          while(currNode->next!=NULL){
            currNode = currNode->next;
            if(strcmp(currNode->path,currentPath)==0){
              option=1;
            }
          }
          if(option==0){
          addStagingList(useStage->head,currentPath,currentDir);
          deleteStagingList(removeFileList->head,currentPath);
          }
          //삭제된 애들의 링크드 리스트에 추가하면 된다.
          //이건 이제 commit할 리스트 만드는 것이기 때문에 사용한다.
          //remove할 때도 없애주면 되는 것이다.
          continue;
        }
        if(S_ISDIR(statbuf2.st_mode)){
            //디렉토리인 경우에 대한 처리를 진행한다.
            //디렉토리인 경우에도 어차피 dirlist만족하는지 확인할 것이기 때문에 상관없다.
            //디렉토리가 있는지 확인하여 없으면 추가해준다.
            if(searchingNode(useDirList->head ,currentPath,0)==1){
              continue;
            }
            addDirList(Queue,useDirList->head,currentPath,useStage->head);
            deleteDir(removeList->head,currentPath,0);
        }
        if(S_ISREG(statbuf2.st_mode)){
          //파일인 경우에 대한 처리를 진행한다.
          //파일이 디렉토리밑이나 스테이징 리스트에 있는지 찾아보고 없으면 추가해준다.
            char *fileName = malloc(sizeof(char)*STRMAX);
            char *currentDir = malloc(sizeof(char)*PATHMAX);
            int idx;

            strcpy(fileName, GetFileName(currentPath));
            for( idx = strlen(currentPath)-1; currentPath[idx] != '/'; idx--);
            strncpy(currentDir,currentPath,idx);
            currentDir[idx] = '\0';

            if(findingStagingNode(useStage->head,currentPath)==1){
              continue;
            }
            if(searchingNode(useDirList->head,currentPath,1)==1){
              continue;
            }
            addStagingList(useStage->head,currentPath,currentDir);
            deleteStagingList(removeFileList->head,currentPath);
        }
      
      }
      else if((substring[i][0] == 'r')){
        char** substring2 = GetSubstring(substring[i],&count2,"\"");
        char *currentPath = malloc(sizeof(char)*PATHMAX);
        strcpy(currentPath,substring2[1]);
        if(lstat(currentPath,&statbuf2)<0){
          //삭제된 애들에 대해서 처리해주면된다.
          char *currentDir = malloc(sizeof(char)*PATHMAX);
          int idx;
          int option=0;
          stagingList *currNode = useStage->head;
          for( idx = strlen(currentPath)-1; currentPath[idx] != '/'; idx--);
          strncpy(currentDir,currentPath,idx);
          currentDir[idx] = '\0';
         if(searchingNode(useDirList->head ,currentPath,0)==1){

            deleteDir(useDirList->head,currentPath,0);
            addDirList(Queue,removeList->head,currentPath,removeFileList->head);
            continue;
           }
          if(searchingNode(useDirList->head,currentPath,1)==1){
              //디렉토리 안에 삭제할 파일이 있는 경우에 대한 것이다.
              deleteNodeInDir(useDirList->head,currentPath);
              addStagingList(removeFileList->head,currentPath,currentDir);
          }

          while(currNode->next!=NULL){
            currNode = currNode->next;
            if(strcmp(currNode->path,currentPath)==0){
              option=1;
            }
          }
          if(option==1){
          addStagingList(removeFileList->head,currentPath,currentDir);
          deleteStagingList(useStage->head,currentPath);
          }
          continue;
        }


        //경로상에 "가 나올 수 없기 때문에 무조건 경로는 배열의 두 번째일 것이다.
        if(S_ISDIR(statbuf2.st_mode)){
          //디렉토리인 경우에 찾아서 하위에 있는 것들을 다 삭제해주면 된다.
            if(findingDirList(removeList->head,currentPath)==1){
              continue;
              //이미 삭제되어 링크드 리스트에 있는 것에 대한 예외처리를 해주는 것입니다.
            }
            if(searchingNode(useDirList->head ,currentPath,0)==1){
              deleteDir(useDirList->head,currentPath,0);
              addDirList(Queue,removeList->head,currentPath,removeFileList->head);
            }
        }
        if(S_ISREG(statbuf2.st_mode)){
          //파일인 경우에는 그 파일을 staging이나 디렉토리의 하위 파일에서 제거해준다.
          char *currentDir = malloc(sizeof(char)*PATHMAX);
          int idx;
          for( idx = strlen(currentPath)-1; currentPath[idx] != '/'; idx--);
            strncpy(currentDir,currentPath,idx);
            currentDir[idx] = '\0';
          if(findingStagingNode(removeFileList->head,currentPath)==1){
               continue;
          }
          if(searchingNode(removeList->head,currentPath,1)==1){
              //디렉토리 안에 삭제할 파일이 있는 경우에 대한 것이다.
              continue;
          }
          if(findingStagingNode(useStage->head,currentPath)==1){
            deleteStagingList(useStage->head,currentPath);
            addStagingList(removeFileList->head,currentPath,currentDir);
          }
          if(searchingNode(useDirList->head,currentPath,1)==1){
              //디렉토리 안에 삭제할 파일이 있는 경우에 대한 것이다.
              deleteNodeInDir(useDirList->head,currentPath);
              addStagingList(removeFileList->head,currentPath,currentDir);
          }
        }//파일의 경우 찾아서 있으면 링크드 리스트에서 삭제시켜 주는 것이다.       
      }
    }
    
  }
 
  close(fd);
  close(fd);
 }//처음 프로그램이 시작될 때 실행되는 함수이다

//메인함수입니다.
int main(int argc, char* argv[]) {
   Init();
   //처음 실행되는 경우 prompt를 실행하고 그 안에서 첫번째 인자를 command나 help로 설정해둠으로써 fork하여 exec하였을 때 해당하는 곳으로 분기되어 실행되게 설정해둔 것이다.
   if(!strcmp(argv[0], "command")) {
    CommandFun(argv+1);
  } else if(!strcmp(argv[0], "help")) {
    printAll();
    }else{
    strcpy(exeNAME, argv[0]); 
   Prompt();
   }
   exit(0);
}
