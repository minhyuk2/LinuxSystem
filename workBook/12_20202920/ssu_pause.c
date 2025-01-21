#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//함수원형입니다.
void ssu_alarm(int signo);

int main(void)
{
	printf("Alarm Setting\n");
	//시그널핸들러를 등록해줍니다.
	signal(SIGALRM,ssu_alarm);
	//SIGALRM을 프로세스에게 전달해줍니다.
	alarm(2);
	
	//pause를 통해 SIGALRM 시그널을 받을 때까지 프로세스의 실행을 중지합니다.
	while(1){
		printf("done\n");
		pause();
		alarm(2);
	}
	
	exit(0);
}

void ssu_alarm(int signo){
	printf("alarm..!!!\n");
}
