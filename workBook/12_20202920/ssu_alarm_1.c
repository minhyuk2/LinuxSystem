#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//함수 원형입니다.
void ssu_signal_handler(int signo);

int count = 0;

int main(void)
{
	//시그널 핸들러를 등록해줍니다.
	signal(SIGALRM, ssu_signal_handler);
	//일 초 후에 SIGALRM 시그널을 발생시킵니다.
	alarm(1);
	//while문을 통해 프로그램이 종료되지 않게 합니다.
	while(1);

	exit(0);
}

void ssu_signal_handler(int signo){
	printf("alarm %d\n",count++);
	alarm(1);
}

