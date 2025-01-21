#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
	sigset_t sig_set;
	int count;

	//시그널 집합을 초기화시켜줍니다.
	sigemptyset(&sig_set);
	//시그널 집합에 SIGINT를 추가시켜줍니다.
	sigaddset(&sig_set,SIGINT);
	//SIG_BLOCK을 인수로 줌으로써 SIGINT를 블록시켜줍니다.
	sigprocmask(SIG_BLOCK, &sig_set,NULL);

	//1초씩 쉬면서 count를 출력해줍니다.
	for(count = 3; 0 < count ; count --){
		printf("count %d\n",count);
		sleep(1);
	}

	printf("Ctrl-C에 대한 블록을 해제\n");
	//SIGINT에 대한 블록을 해제시켜줍니다.
	sigprocmask(SIG_UNBLOCK,&sig_set,NULL);
	printf("count중 Ctrl-C입력하면 이 문장은 출력 되지 않음.\n");

	while(1);

	exit(0);
}

