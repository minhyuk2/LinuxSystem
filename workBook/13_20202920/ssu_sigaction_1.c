#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo){
	printf("ssu_signal_handler control\n");
}

int main(void){
	
	struct sigaction sig_act;
	sigset_t sig_set;

	//sig_act의 sa_mask를 초기화시켜줍니다.
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	
	//handler를 등록시켜줍니다
	sig_act.sa_handler = ssu_signal_handler;
	//SIGUSR1에 대한 액션에 댛나 정보를 전달해줍니다.
	sigaction(SIGUSR1, &sig_act, NULL);
	printf("before first kill()\n");
	//현재 프로세스에게  SIGUSR1을 보내줍니다.
	kill(getpid(),SIGUSR1);
	sigemptyset(&sig_set);
	sigaddset(&sig_set,SIGUSR1);
	//해당 시그널 집합에 있는 것들을 블록시켜줍니다.
	sigprocmask(SIG_SETMASK,&sig_set,NULL);
	printf("before second kill()\n");
	kill(getpid(),SIGUSR1);
	printf("after second kill()\n");
	exit(0);
}
