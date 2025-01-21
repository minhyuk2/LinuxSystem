#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void ssu_signal_handler1(int signo);
static void ssu_signal_handler2(int signo);

int main(void)
{
	struct sigaction act_int, act_quit;

	//handler를 설정해줍니다.
	act_int.sa_handler = ssu_signal_handler1;
	//sa_mask를 초기화시켜줍니다.
	sigemptyset(&act_int.sa_mask);
	sigaddset(&act_int.sa_mask , SIGQUIT);
	act_quit.sa_flags = 0;

	//sigaction으로 sigint에 대해서 설정시켜줍니다.
	if(sigaction(SIGINT, &act_int, NULL) < 0){
		fprintf(stderr,"sigaction(SIGINT) error\n");
		exit(1);
	}

	//handler를 등록해줍니다
	act_quit.sa_handler = ssu_signal_handler2;
	sigemptyset(&act_quit.sa_mask);
	sigaddset(&act_quit.sa_mask,SIGINT);
	//flag를 설정해줍니다
	act_int.sa_flags = 0;
	//SIGQUIT일때의 sigaction을 설정해줍니다.
	if(sigaction(SIGQUIT,&act_quit,NULL) < 0){
		fprintf(stderr,"sigaction(SIGQUIT) error\n");
		exit(1);
	}

	pause();
	exit(0);
}

//함수에 대한 정의를 해줍니다.
static void ssu_signal_handler1(int signo){
	printf("Signal handler of SIGINT : %d\n",signo);
	printf("SIGQUIT signal is blocked : %d\n",signo);
	printf("sleeping 3 sec\n");
	sleep(3);
	printf("Signal handler of SIGINT ended\n");
}

static void ssu_signal_handler2(int signo){
	printf("Signal handler of SIGQUIT : %d\n",signo);
	printf("SIGINT signal is blocked : %d\n",signo);
	printf("sleeping 3 sec\n");
	sleep(3);
	printf("Signal handler of SIGQUIT ended\n");
}

