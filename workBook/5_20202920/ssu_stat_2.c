#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

struct stat statbuf;

void ssu_checkfile(char *fname, time_t *time);
//함수원형을 적어두었다.

int main(int argc, char*argv[])
{
	time_t intertime;
	
	if(argc != 2){
		fprintf(stderr, "usage: %s <file>\n",argv[0]);
		exit(1);
	}
	//인자의 개수가 2개가 아니면 프로그램을 종료한다.

	 if(stat(argv[1],&statbuf)<0){
		 fprintf(stderr,"stat error for %s\n",argv[1]);
		 exit(1);
	 }
	 //stat함수를 사용해서 stat구조체에 대한 정보를 얻어낸다.
	intertime = statbuf.st_mtime;
	while(1){
		ssu_checkfile(argv[1],&intertime);
		sleep(10);
	} //10초마다 계속 반복되면서 ssu_checkfile을 실행시켜서 file이 수정되었는지 확인한다.
}

void ssu_checkfile(char *fname, time_t *time){
	if(stat(fname,&statbuf)<0){
		fprintf(stderr,"Warning : ssu_checkfile() error!\n");
		exit(1);
	}
	//stat함수를 써서 stat 구조체를 갖고온다.
	else 
		if(statbuf.st_mtime != *time){
			printf("Warning : %s was modified!.\n",fname);
			*time = statbuf.st_mtime;
		} //위에서 받아온 stat구조체의 mtime이 매개변수로 받은 time과 다른경우에 변환되었다고 출력하고 time 을 수정해준다.
}
