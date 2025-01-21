#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void ssu_signal_handler(int signo);
void ssu_timestamp(char *str);

int main(void)
{
	struct sigaction sig_act;
	sigset_t blk_set;

	//fillset으로 blk_set을 채워줍니다.
	sigfillset(&blk_set);
	sigdelset(&blk_set,SIGALRM);
	//sigempty로 비워줍니다.
	sigemptyset(&sig_act.sa_mask);
	//flag를 설정해줍니다.
	sig_act.sa_flags = 0;
	//handler를 설정해줍니다.
	sig_act.sa_handler = ssu_signal_handler;
	//SIGALARM에 대한 핸들러를 설정해줍니다.
	sigaction(SIGALRM,&sig_act,NULL);
	ssu_timestamp("before sigsuspend()");
	alarm(5);
	//시그널이 발생할때까지 팬딩시켜줍니다.
	sigsuspend(&blk_set);
	ssu_timestamp("after sigsuspend()");
	exit(0);
}

void ssu_signal_handler(int signo){
	printf("int ssu_signal_handler() function\n");
}

void ssu_timestamp(char *str){
	time_t time_val;

	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}

