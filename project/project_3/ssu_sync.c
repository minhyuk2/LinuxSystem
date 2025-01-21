#include "ssu_header.h"

//list명령어에 대한 것입니다.
int ListCommand(command_parameter *parameter){

  FILE *fp;
  FILE *file;
  int fd22;
  char* buffer= (char*)malloc(sizeof(char)*PATHMAX*2);
  char *getPid = (char *)malloc(sizeof(char)*BUF_MAX);
  char *getPath = (char *)malloc(sizeof(char)*PATHMAX); //로그에 적힌 경로입니다.
  char *bufferPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *forLogPath = (char *)malloc(sizeof(char)*PATHMAX);
  int isOk = 0;

  //그냥 list만 입력받은 경우입니다.
  if(strcmp(parameter->filename,"all")==0){
    if((fp= fopen(logPATH,"r"))==NULL){
      fprintf(stderr,"fopen error %s\n",logPATH);
      return 1;
    }
    while(fgets(buffer,PATHMAX*2,fp)!=NULL){
      printf("%s",buffer);
    }
    return 0;
  }
  
  //해당 PID에 대한 daemon processs가 monitoring하는 것들의 트리구조입니다.

  //로그파일에서 해당 PID에 대한 로그가 존재하는지 확인합니다.
  if((fp = fopen(logPATH,"r"))==NULL){
    fprintf(stderr, "fopen error %s\n",logPATH);
    return 1;
  }
  while(fgets(buffer,PATHMAX*2,fp)!=NULL){
      sscanf(buffer,"%s : %s\n",getPid,getPath);
      if(strcmp(getPid,parameter->filename)==0){
        isOk = 1;
        break;
      }
  }

  fclose(fp);
  //pid가 없을 때에 대한 에러처리입니다.
  if(isOk == 0){
    printf("ERROR: There is no daemon process for entered PID\n");
    return 0;
  }

  //PID에 대한 로그파일을 열어줍니다.
  sprintf(forLogPath,"%s/%s.log",backupPATH,getPid);
  
  if((fd22 = open(forLogPath,O_RDONLY))<0){
    fprintf(stderr, "open error \n");
    return 1;
  }
    //원본디렉토리나 파일의 경로를 넣어서 init_backup_list함수를 수행시켜줍니다.
    getPath[strlen(getPath)] = '\0';
    init_backup_list(fd22,getPath);
    close(fd22);
}

//Remove 명령어입니다.
int RemoveCommand(command_parameter *parameter){
  FILE *fp;
  FILE *fp2;
  pid_t tempPid;
  char **buflist = NULL;
  char *logBuffer = (char*)malloc(sizeof(char)*BUF_MAX);
  char *buffer = (char*)malloc(sizeof(char)*PATHMAX*2);
  int cnt;
  char *logPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *tempPath = (char *)malloc(sizeof(char)*PATHMAX);
  char *erasingPath = (char *)malloc(sizeof(char)*PATHMAX);
  char printPath[PATHMAX];

  if((fp = fopen(logPATH,"r+"))==NULL){
    fprintf(stderr, "fopen error %s\n",logPATH);
    return 1;
  }
  //로그에 적혀있는 것을 지우기 위해 임시파일을 생성하여 지워야할 부분을 제외하고 내용을 옮겨서 적어줍니다.
  sprintf(erasingPath,"%s/realrealtemp.txt",exePATH);
  if((fp2 = fopen(erasingPath,"w+"))==NULL){
    fprintf(stderr, "fopen error %s\n",erasingPath);
    return 1;
  }

  while(fgets(buffer,PATHMAX*2,fp)!=NULL){
    buflist = GetSubstring(buffer,&cnt," ");
    if(strcmp(buflist[0],parameter->filename)==0){
      //같은 해당하는 pid가 있는 경우
      strcpy(printPath,buflist[2]);
      //해당하는 문장을 제외하고 다시 적어준다.
      while(fgets(buffer,PATHMAX*2,fp)!=NULL){
        fputs(buffer,fp2);
      }
      fclose(fp);
      if((fp = fopen(logPATH,"w"))==NULL){
        fprintf(stderr, "fopen error %s\n",logPATH);
        fclose(fp2);
        remove(erasingPath);
        return 1;
      }
      fseek(fp2,0,SEEK_SET);
      while(fgets(buffer,PATHMAX*2,fp2)!=NULL){
        fputs(buffer,fp);
      }
      fclose(fp2);
      remove(erasingPath);
      
      //해당하는 pid에 대한 로그를 삭제해준다.
      sprintf(tempPath,"%s/%s.log",backupPATH,parameter->filename);
      remove(tempPath);
      

      //백업파일들을 전부삭제해준다.
      sprintf(tempPath,"%s/%s",backupPATH,parameter->filename);
      if(deleteDirectory(tempPath) == 1){
        fprintf(stderr, "directory delete error\n");
        return 1;
      }

      //kill명령어로 daemon process에 signal을 보냅니다.
      if((tempPid = atoi(parameter->filename))==0){
        fprintf(stderr, "atoi error\n");
        return 1;
      }
      if(kill(tempPid, SIGUSR1)<0) {
        fprintf(stderr,"kill error\n");
        return 1;
      }
      printPath[strlen(printPath)-1] = '\0';
      printf("monitoring ended (%s) : %s\n",printPath,parameter->filename);
      fclose(fp);
      return 0;
    }
    fputs(buffer,fp2);
  }
  fclose(fp2);
  //만들어준 임시파일을 삭제해줍니다.
  remove(erasingPath);
  //입력받은 PID가 없는 경우에 에러를 출력시켜줍니다.
  printf("ERROR : Wrong PID\n");
  fclose(fp);
  return 0;
}

//Add명령어입니다.
int AddCommand(command_parameter *parameter){
  struct stat buf;
  pid_t pids;
  FILE *fp;
  FILE *fp2;
  unsigned int time;
  char **buflist = NULL;
  char *buffer = (char*)malloc(sizeof(char)*PATHMAX*2);
  char *logBuffer = (char*)malloc(sizeof(char)*BUF_MAX);
  char *currentPath = (char*)malloc(sizeof(char)*PATHMAX);
  char *fileName = (char*)malloc(sizeof(char)*FILE_MAX);
 
  strcpy(currentPath,parameter->filename);
  strcpy(fileName , GetFileName(currentPath));


  if(lstat(parameter ->filename,&buf)<0){
    fprintf(stderr,"lstat error %s\n",parameter->filename);
    return 1;
  }
  //파일인 경우와 디렉토리인 경우에 다르게 처리해주면 됩니다.
  //파일과 디렉토리 둘 다 아닌 경우에 대한 예외처리입니다.
  if(!S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode)) {
    fprintf(stderr, "ERROR: %s is not directory or regular file\n", parameter->filename);
    return 1;
  }

  //-r이나 -d 옵션인 경우
  if((parameter->commandopt&OPT_D)||((parameter->commandopt&OPT_R))){
    //-d나 -r이나 따라 다르게 되게끔 우선적으로 설정해줍니다.
  }else{
    //디렉토리인 경우에 대한 예외를 처리해줍니다.
    if(S_ISDIR(buf.st_mode)) {
      fprintf(stderr, "\"%s\" is  directory\n", parameter->filename);
      return 1; 
    }
  }
  if(((parameter->commandopt&OPT_R)!=0)||(parameter->commandopt&OPT_D)!=0){
      if(S_ISREG(buf.st_mode)) {
        fprintf(stderr, "\"%s\" is file\n", parameter->filename);
        return 1; 
      }
  }

  //-t옵션이 있는 경우 parameter로 입력받은 time에 대해서 처리해줍니다.
  if((parameter->commandopt&(1 << 2))!= 0){
    time = atoi(parameter->tmpname);
  }else{
    time = 1;
  }

  //이미 파일이 모니터링
  // 로그파일에 존재하는 경우에 대한 예외처리입니다. 
  if((fp= fopen(logPATH,"r+"))==NULL){
    fprintf(stderr, "fopen error %s\n",logPATH);
    return 1;
  }
  while (fgets(buffer,PATHMAX*2, fp) != NULL) {
        int cnt;
        buffer[strlen(buffer)] = '\0';
        buflist = GetSubstring(buffer,&cnt," ");
        //이미 존재하는 파일이 있는 경우에 대한 예외처리입니다.
        buflist[2][strlen(buflist[2])-1]='\0';
        if(strcmp(buflist[2],parameter->filename)==0){
          printf("\"%s\" is already monitored\n",parameter->filename);
          fclose(fp);
          return 0;
      }
  }
 fclose(fp);
 
 //daemon 프로세스를 생성하는 함수를 호출시켜줍니다.
 if((parameter->commandopt&OPT_R)!=0){
  //-r옵션인 경우입니다.
  //-r과 -d옵션이 같이 쓰여도 -r옵션이 선택된 것으로써 처리합니다.
  ssu_daemon_init_dirR(currentPath,fileName,time);

 }else if ((parameter->commandopt&OPT_D)!=0){
  //-d옵션인 경우입니다.

  ssu_daemon_init_dir(currentPath,fileName,time);
 }else{
  //옵션 없이 파일만 들어간 경우입니다.
  ssu_daemon_init(currentPath,fileName,time);
 }
 
 fflush(stdout);
}

//help와 help add와 같은 다른 명령어가 입력되었을 경우에 대한 처리입니다.
int HelpCommand(command_parameter *parameter){
  char *input = (char *)malloc(sizeof(char)*STRMAX);
  strcpy(input,parameter->filename);
  if(strcmp("add",parameter->filename)==0){
    printUsage();
    printAdd();
    return 0;
  }else if(strcmp("remove",parameter->filename)==0){
    printUsage();
    printRemove();
    return 0;
  }
  else if(strcmp("list",parameter->filename)==0){
    printUsage();
    printHelp();
    return 0;
  }
 else if(strcmp("help",parameter->filename)==0){
    printUsage();
    printHelp();
    return 0;
  }else if(strcmp("exit",parameter->filename)==0){
    printUsage();
    printExit();
    return 0;
  }
  else{
    printAll();
    return 0;
  }
  return 0;
}

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

//fork()되었을 때 어떤 함수를 실행할지 설정해주는 함수입니다.
void CommandFun(char **arglist) {
  int (*commandFun)(command_parameter * parameter);
  //함수 포인터를 생성하는 것이다.
	command_parameter parameter={
    arglist[0], arglist[1], arglist[2], atoi(arglist[3])
  };

  if(!strcmp(parameter.command, commanddata[0])) {
    commandFun = AddCommand;
  }
   else if(!strcmp(parameter.command, commanddata[1])) {
    commandFun = RemoveCommand;
   }
  else if(!strcmp(parameter.command, commanddata[2])) {
    commandFun = ListCommand;
  }
   else if(!strcmp(parameter.command, commanddata[3])) {
    commandFun = HelpCommand;
  }

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
  parameter.argv[4] = (char *)malloc(sizeof(char *) * 32);
  sprintf(parameter.argv[4], "%d", parameter.commandopt);
  parameter.argv[5] = (char *)0;
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


//인자로 받은 것에 대한 처리를 진행해주는 함수입니다.
int ParameterProcessing(int argcnt, char **arglist, int command, command_parameter *parameter) {
    struct stat buf;
    struct stat statbuf;
    int fd;
    char *logString = (char *)malloc(sizeof(char)*(PATHMAX+8));
    char *relativePath = (char *)malloc(sizeof(char) *PATHMAX);
    int optcnt = 0;
    int option;
    int lastind;
    optind = 0;
    opterr = 0;
    switch(command){
        case CMD_ADD: {
          //ADD의 인자로 넣어줄 것에 대한 것들의 예외처리를 해주는 것입니다.
          if (argcnt < 2) {
			        	fprintf(stderr,"ERROR: <PATH> is not include\n");
                printAdd();
			      	return -1;
	      		}
          //경로가 존재하지 않는 경우에 에러메세지를 출력합니다.
          if(realpath(arglist[1],parameter->filename)==NULL) {
                printf("ERROR: \'%s\' is wrong path\n",parameter->filename);
                return -1;
           }
           //또한 이때 arglist[1]의 값을 parameter->filename에 넣어줍니다.
          //lstat를 통해 존재하는지 한 번 더 확인해줍니다.
          if(lstat(parameter->filename, &buf) < 0) {
                fprintf(stderr, "ERROR: lstat error for %s\n", parameter->filename);
                return -1;
          }
          //현재 홈디렉토리 외의 다른 경로에 있는 경우에 대한 에러처리를 해줍니다.
          if(strstr(parameter->filename,homePATH)==NULL){
                printf("ERROR: \'%s\' is wrong path\n",parameter->filename);
                return -1;
          }
          //백업디렉토리를 포함하는 경로가 들어온 경우에 대한 에러처리를 해줍니다.
          if(strstr(parameter->filename,backupPATH)!=NULL){
            printf("ERROR: \'%s\' is wrong path\n",parameter->filename);
            return -1;
          }
          
          lastind = 1;

          while((option = getopt(argcnt, arglist, "drt")) != -1) {
            
            if(option != 'd' && option != 'r'&& option != 't') {
              fprintf(stderr, "ERROR: unknown option \n");
              return -1;
            }

            if(optind == lastind) {
              fprintf(stderr, "ERROR: wrong option input\n");
              return -1;
            }
            //같은 옵션이 두 번 들어오는 경우에 대한 예외처리입니다.
            if(option == 'd')	{
              if(parameter->commandopt & OPT_D) {
                fprintf(stderr, "ERROR: duplicate option -%c\n", option);
                return -1;
              }
              parameter->commandopt |= OPT_D;
            }
            
            if(option == 'r')	{
              if(parameter->commandopt & OPT_R) {
                fprintf(stderr, "ERROR: duplicate option -%c\n", option);
                return -1;
              }
              parameter->commandopt |= OPT_R;
            }
           if(option == 't')	{
              if(parameter->commandopt & OPT_T) {
                fprintf(stderr, "ERROR: duplicate option -%c\n", option);
                return -1;
              }
              parameter->commandopt |= OPT_T;
            }
            optcnt++;
            lastind = optind;
          }

          //option 비트를 설정하는 것을 통해서 어떤 option들이 들어갔는지 처리할 수 있다.
          if((parameter->commandopt&OPT_T)!= 0){
            //t옵션인 경우 같이 들어오는 옵션의 개수가 몇 개인지에 따라 들어오는 시간을 다르게 처리해주면 됩니다.
            if(argcnt != optcnt +3){
              printUsage();
              printAdd();
              return -1;
            }
            strcpy(parameter->tmpname,arglist[optcnt+2]);
          }else{
            if((argcnt > optcnt+2)){
              printUsage();
              printAdd();
              return -1;
            }
          }
          break;
        }
        case CMD_REM : {
            //remove명령어가 입력되었을 때 해줄 수 있는 예외처리입니다.
            if (argcnt < 2) {
				        fprintf(stderr,"ERROR: <DAEMON PID> is not include\n");
                printUsage();
                printRemove();
			        	return -1;
		        	}
            if(argcnt > 2){
                fprintf(stderr,"Wrong Input\n");
                printUsage();
                printRemove();
			        	return -1;
            }
            strcpy(parameter->filename,arglist[1]);
            break;
        }
        case CMD_LIST :{
          //list가 명령어로 들어왔을 때에 대한 처리를 해주면 되는 것이다.
          if(argcnt > 2){
            printUsage();
            printList();
            return -1;
          }
          if(argcnt == 2){
            //PID를 입력받은 경우
            strcpy(parameter->filename,arglist[1]);
          }
          else{
            //PID를 입력받지 않아서 전부 출력하는 경우
            strcpy(parameter->filename,"all");
          } 
          break;
        }
        case CMD_HELP : {
          
        if(argcnt == 2){
          strcpy(parameter->filename,arglist[1]);
        }else if(argcnt > 2){
          printf("ERROR : too much command\n");
          printUsage();
          printHelp();
          return -1;
        }
        else{
           strcpy(parameter->filename,"not");
        }
        break;
        }
    }
} //인자의 개수와 매개변수로 들어온 파일에 대한 예외처리를 미리 해주었다.



//parameter를 초기화해주는 함수입니다.
void ParameterInit(command_parameter *parameter) {
  parameter->command = (char *)malloc(sizeof(char *) * PATHMAX);
  parameter->filename = (char *)malloc(sizeof(char *) * PATHMAX);
  parameter->tmpname = (char *)malloc(sizeof(char *) * PATHMAX);
  parameter->commandopt = 0;
} 

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
	if(!strcmp(arglist[0], commanddata[0])) {
			command = CMD_ADD;
		} else if(!strcmp(arglist[0], commanddata[1])) {
			command = CMD_REM;
		}	else if(!strcmp(arglist[0], commanddata[2])) {
			command = CMD_LIST;
		}	else if(!strcmp(arglist[0], commanddata[3])) {
			command = CMD_HELP;
		}	else if(!strcmp(arglist[0], commanddata[4])) {
			command = CMD_EXIT;
          exit(0);
          //exit이 들어오면 프로그램 종료
		}	else {
			command = NOT_CMD;
		}
        
    if(command & (CMD_ADD | CMD_REM |CMD_LIST| CMD_HELP)) {
	  	  ParameterInit(&parameter);
        parameter.command = arglist[0];
      //command에 어떤 command인지 넣어준다.
      if(ParameterProcessing(argcnt, arglist, command, &parameter) == -1) {
        continue;
      } //-1이 리턴되면 설정된 일부분에서 오류가 발생한 것이기 때문에 -1을 리턴한다.
      CommandExec(parameter);

    }else if(command == NOT_CMD) {
        //help명령어를 출력해주게 설정해주면 되는 것이다.
        HelpExec();
    }
  }
}




//메인함수입니다.
int main(int argc, char* argv[]) {
   Init();
   //처음 실행되는 경우 prompt를 실행하고 그 안에서 첫번째 인자를 command나 help로 설정해둠으로써 fork하여 exec하였을 때 해당하는 곳으로 분기되어 실행되게 설정해둔 것이다.
   if(!strcmp(argv[0], "command")) {
    CommandFun(argv+1);
  }else if(!strcmp(argv[0], "help")) {
    printAll();
  }
  else{
   strcpy(exeNAME, argv[0]); 
   Prompt();
   }
   exit(0);
}
