#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//함수 원형들입니다.
void ssu_signal_handler1(int signo);
void ssu_signal_handler2(int signo);

int main(void)
{
	//signal함수를 사용하여 시그널 핸들러를 등록해줍니다.
	if(signal(SIGINT,ssu_signal_handler1) == SIG_ERR){
		fprintf(stderr,"cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	//signal함수를 사용하여 시그널 핸들러를 등록해줍니다.
	if(signal(SIGUSR1, ssu_signal_handler2) == SIG_ERR){
		fprintf(stderr, "cannot handle SIGUSR1\n");
		exit(EXIT_FAILURE);
	}
	//raist함수를 통해 자기 자신에게 signal을 보냅니다.
	raise(SIGINT);
	raise(SIGUSR1);
	printf("main return\n");
	exit(0);
}

void ssu_signal_handler1(int signo){
	printf("SIGINT 시그널 발생\n");
}

void ssu_signal_handler2(int signo){
	printf("SIGUSR1 시그널 발생\n");
}
