#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	//인자가 2개가 아닌 경우에는 에러 메세지를 출력합니다.
	if(argc != 2){
		fprintf(stderr, "usage: %s [Process ID]\n",argv[0]);
		exit(1);
	}
	else
		kill(atoi(argv[1]),SIGKILL);//인자로 들어온 PID에 SIGKILL을 보냅니다.

	exit(0);
}

