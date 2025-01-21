#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//함수 원형들입니다.
void ssu_signal_handler(int signo);
void (*ssu_func)(int);

int main(void){
	//SIGINT에 대한 시그널을 어떻게 처리할지 설정해줍니다.
	ssu_func = signal(SIGINT,ssu_signal_handler);
	
	//1초씩 sleep하면서 무한루프를 돕니다.
	while(1){
		printf("process running...\n");
		sleep(1);
	}
	
	exit(0);
}

void ssu_signal_handler(int signo){
	printf("SIGINT 시그널 발생.\n");
	printf("SIGINT를 SIG_DFL로 재설정 함.\n");
	//SIGINT처리에 대한 함수를 설정해줍니다
	signal(SIGINT,ssu_func);
}

