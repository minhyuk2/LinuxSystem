#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	if(fork() == 0)
		execl("/bin/echo", "echo", "this is", "messsage one", (char *)0);
	//자식프로세스의 경우 execl함수를 수행시킵니다.

	if(fork() == 0)
		execl("/bin/echo", "echo", "this is", "message Two", (char *)0);

	printf("parent: waiting for children\n");

	while(wait((int *)0) != -1);
	//부모 프로세스가 모든 자식 프로세스의 종료를 기다립니다.

	printf("parent: all children terminated\n");
	exit(0);
}
