#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){

	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int fd;
	int length;

	if((fd = open(fname,O_RDONLY,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}
	//ssu_test.txt파일을 오픈한다.

	if(dup2(1,4) != 4){
		fprintf(stderr,"dup2 call failed\n");
		exit(1);
	}
	//표준 출력인 1번 파일 디스크립터를 4번으로 복사한다.

	while(1){
		length = read (fd, buf, BUFFER_SIZE);
		//버퍼 사이즈만큼 읽어서 buf에 저장한다.

		if(length<=0)
			break;
		write(4,buf,length);
		//버퍼에 저장한 것들을 4번 파일디스크립터에 적는다.
	}
	exit(0);
}
