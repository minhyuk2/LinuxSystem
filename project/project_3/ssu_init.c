#include "ssu_header.h"
//프로세스가 실행될 때 처음으로 실행되는 함수와 관련된 부분입니다.

void Init(){

    int fd;
    char *backupPath = (char *)malloc(sizeof(char)*PATHMAX);

    getcwd(exePATH, PATHMAX);
    //현재 실행중인 파일의 경로를 알려준다.
    sprintf(homePATH, "%s", getenv("HOME"));
    //home 경로를 알려준다.
    
    sprintf(backupPath, "%s/%s",homePATH,"backup");
    strcpy(backupPATH,backupPath);
    if(access(backupPath, F_OK)!=0){
        mkdir(backupPath, 0777);
    }//backup 디렉토리가 없으면 생성해준다.
    
    sprintf(logPATH, "%s/%s", backupPath, "monitor_list.log");


    //log파일이 없으면 생성해줍니다.
    if((fd = open(logPATH,O_RDWR|O_CREAT,0777))<0){
    fprintf(stderr,"error cause from %s\n",logPATH);
	exit(1);
    }
    close(fd);
}
