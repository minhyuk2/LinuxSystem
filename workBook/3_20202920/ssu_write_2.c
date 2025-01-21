#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define S_MODE 0664
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	char buf[BUFFER_SIZE];
	int fd1, fd2;
	int length;

	if(argc != 3){
		 fprintf(stderr, "Usage : %s filein fileout\n",argv[0]);
				 exit(1);
	}// main의 매개변수로 받은 것이 3개가 아니면 에러로 처리한다.
	if((fd1 = open(argv[1],O_RDONLY))< 0){
		 fprintf(stderr,"open error for %s\n",argv[1]);
		 exit(1);
	}// 두번째 매개변수를 readonly로 연다.

	if((fd2 = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,S_MODE))<0){
		fprintf(stderr, "open error for %s\n",argv[2]);
		exit(1);
	}// 세번째 인자로 받은 것을 여는데 없으면 만들고 있으면 초기화시킨다.

	while ((length = read(fd1,buf,BUFFER_SIZE))>0)
		write(fd2,buf,length);
	//fd1으로 연 것의 내용을 fd2에 적어준다.

	 exit(0);
}
