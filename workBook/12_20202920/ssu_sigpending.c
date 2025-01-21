#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
	sigset_t pendingset;
	sigset_t sig_set;

	int count = 0;

	//모든 시그널을 sig_set에 추가해줍니다.
	sigfillset(&sig_set);
	//시그널 집합의 모든 시그널을 블록시켜줍니다.
	sigprocmask(SIG_SETMASK, &sig_set, NULL);

	//무한으로 반복하면서 count를 출력해줍니다.
	while(1){
		printf("count: %d\n",count++);
		sleep(1);
		//현재 팬딩 중인 시그널 집합의 정보를 가져옵니다.
		if(sigpending(&pendingset) == 0){
			if(sigismember(&pendingset,SIGINT)){
				//팬딩 중인 시그널 집합중에서 SIGINT가 있는 경우에는 무한루프를 빠져나오며 프로그램을 종료합니다.
				printf("SIGINT가 블록되어 대기 중, 무한 루프를 종료.\n");
				break;
			}
		}
	}
	exit(0);
}
