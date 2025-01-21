#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
	sigset_t old_set;
	sigset_t sig_set;

	//emptyset으로 설정시켜줍니다.
	sigemptyset(&sig_set);
	//SIGINT를 등록시켜줍니다.
	sigaddset(&sig_set,SIGINT);
	//promask로 블록시켜줍니다.
	sigprocmask(SIG_BLOCK, &sig_set, &old_set);
	sigsuspend(&old_set);
	exit(0);

}

