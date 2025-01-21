#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){

	char *fname = "ssu_test.txt";
	int fd;

	if((fd = creat(fname,0666)) < 0){
		printf("creat error for %s\n",fname);
		exit(1);
	}
	//ssu_test.txt파일을 creat함수를 통해서 만들어준다.

	printf("First printf is on the screen.\n");
	dup2(fd,1);
	//fd를 표준 출력 1번 파일 디스크립터로 복사해준다.
	printf("Second printf is in this file.\n");
	exit(0);
}
