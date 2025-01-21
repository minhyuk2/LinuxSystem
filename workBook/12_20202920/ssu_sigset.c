#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(void)
{
	sigset_t set;
	//시그널 집합을 초기화시켜줍니다.
	sigemptyset(&set);
	//시그널 집합에 SIGINT를 추가해줍니다.
	sigaddset(&set,SIGINT);
	

	//시그널 집합에 SIGINT가 있는 경우와 없는 경우에 대한 케이스를 나눠서 각기 다르게 출력한다.
	switch(sigismember(&set,SIGINT))
	{
		case 1  :
			printf("SIGINT is included. \n");
			break;
		case 0 :
			printf("SIGINT is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
	}
	//시그널 집합에 SIGSYS가 포함되어있는지 확인하여 각기 다른 결과를 출력합니다.
	switch (sigismember(&set,SIGSYS))
	{
		case 1 :
			printf("SIGSYS is included. \n");
			break;
		case 0 :
			printf("SIGSYS is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
	}

	exit(0);
}
