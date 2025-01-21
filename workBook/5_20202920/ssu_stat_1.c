#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
	struct stat statbuf;

	if(argc != 2){
		fprintf(stderr, "usage : %s <file>\n",argv[0]);
		exit(1);
	}
	//인자가 두 개가 아니면 프로그램을 종료한다.	
	if((stat(argv[1], &statbuf))<0){
		fprintf(stderr, "stat error\n");
		exit(1);

	}//stat 함수를 사용해서 stat구조체를 얻는다.
	printf("%s is %ld bytes\n",argv[1],statbuf.st_size);//stat구조체에 담긴 정보를 출력한다.
	exit(0);
}

