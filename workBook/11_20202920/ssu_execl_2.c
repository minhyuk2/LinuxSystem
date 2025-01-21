#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	if(fork() == 0){
		execl("/bin/echo","echo","this is","message one",(char*)0);
		fprintf(stderr,"exec error\n");
		exit(1);
	}
	//fork로 자식 프로세스를 생성하고 execl로 인자를 넣어줍니다.

	if(fork() == 0){
		execl("/bin/echo","echo","this is","message two",(char*)0);
		fprintf(stderr,"exec error\n");
		exit(1);
	}
	//위의 과정을 인자 하나를 바꾼 채 진행해줍니다.

	if(fork()==0){
		execl("/bin/echo","echo","this is", "message three",(char*)0);
		fprintf(stderr,"exec error\n");
		exit(1);
	}
	//위의 과정을 인자 하나를 바꾼 채 진행해줍니다.

	printf("Parent program ending\n");
	exit(0);
}

