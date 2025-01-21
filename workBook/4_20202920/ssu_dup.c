#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(void){

	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int count;
	int fd1, fd2;

	if((fd1 = open(fname,O_RDONLY,0644))<0){
			fprintf(stderr,"open error for %s\n",fname);
			exit(1);
			}//ssu_test.txt파일을 읽기전용으로 오픈한다.

	fd2 = dup(fd1);
	//fd1의 파일 디스크립터를 복사해서 fd2에 넣어준다.
	count = read(fd1,buf,12);
	//fd1에서 12byte를 읽어서 buf에 저장시킨다.
	buf[count] = 0;
	//buf에 널 문자를 추가해준다.
	printf("fd1's printf : %s\n",buf);
	//buf에 저장된 문자열을 출력한다.
	lseek(fd1,1,SEEK_CUR);
	//fd1의 오프셋위치를 한 칸 이동시킨다.
	count = read(fd2,buf,12);
	buf[count]=0;
	printf("fd2's printf : %s\n",buf);
	//fd2에 대해서 fd1을 통해 행했던 것을 똑같이 진행한다.
	exit(0);
}

