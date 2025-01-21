#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void ssu_signal_handler(int signo);

jmp_buf jump_buffer;

int main(void){
	//SIGINT에 대한 핸들러를 등록해줍니다.
	signal(SIGINT,ssu_signal_handler);

	//jump될 위치를 설정해줍니다.
	while(1){
		if(setjmp(jump_buffer) == 0){
			printf("Hit Ctrl-c at anytime...\n");
			pause();
		}
	}
	exit(0);
}

void ssu_signal_handler(int signo)
{
	char character;
	
	//시그널을 무시하도록등록해줍니다
	signal(signo,SIG_IGN);
	printf("Did you hit Ctrl-c?\n" "Do you really want to quit? [y/n] ");

	character = getchar();

	if(character == 'y' || character == 'Y')
		exit(0);
	else{
		signal(SIGINT,ssu_signal_handler);
		longjmp(jump_buffer,1);
	}
}
