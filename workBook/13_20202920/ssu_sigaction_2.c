#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


//함수원형들입니다.
void ssu_check_pending(int signo, char *signame);
void ssu_signal_handler(int signo);

int main(void)
{
	struct sigaction sig_act;
	sigset_t sig_set;

	//sig_act의 sa_mask를 변경시켜줍니다.
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	//handler를 등록시켜줍니다.
	sig_act.sa_handler = ssu_signal_handler;

	//sigaction으로 SIGUSR1이 들어오는 것에 대한 처리를 등록해줍니다.
	if(sigaction(SIGUSR1, &sig_act ,NULL)!=0){
		fprintf(stderr,"sigaction() error\n");
		exit(1);
	}
	else{
		//sig_set을 비워줍니다.
		sigemptyset(&sig_set);
		sigaddset(&sig_set,SIGUSR1);
		
		//sigprocmask로 SIGUSR1을 블록시켜줍니다.
		if(sigprocmask(SIG_SETMASK, &sig_set,NULL)!=0){
			fprintf(stderr,"sigprocmask() error\n");
			exit(1);
		}
		else {
			//sigusr1을 보내줍니다.
			printf("SIGUSR1 signals are now blocked\n");
			kill(getpid(),SIGUSR1);
			printf("after kill()\n");
			//시그널이 블록된 상태를 확인합니다.
			ssu_check_pending(SIGUSR1,"SIGUSR1");
			sigemptyset(&sig_set);
			//sigpromask로 설정해줍니다.
			sigprocmask(SIG_SETMASK, &sig_set,NULL);
			printf("SIGUSR1 signals are no longer blocked\n");
			//시그널 블록된 상태를 확인합니다.
			ssu_check_pending(SIGUSR1,"SIGUSR1");
		}
	}
	exit(0);
}

void ssu_check_pending(int signo,char *signame){
	sigset_t sig_set;

	//팬딩시켜줍니다
	if(sigpending(&sig_set) != 0)
		printf("sigpending() error\n");
	else if(sigismember(&sig_set,signo))
		//signo가 있는 경우입니다
		printf("a %s siganl is pending\n",signame);
	else
		printf("%s siganls are not pending\n",signame);
}

//함수를 정의시켜줍니다.
void ssu_signal_handler(int signo){
	printf("in ssu_signal_hanlder function\n");
}

