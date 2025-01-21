#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);

int main(void){

	char buf[LINE_MAX];
	int n;
	//시그널 핸들러를 등록해줍니다.
	if(signal(SIGALRM,ssu_alarm) == SIG_ERR){
		fprintf(stderr,"SIGALRM error\n");
		exit(1);
	}
	//10초 후에 SIGALRM을 프로세스에게 전달시켜줍니다.
	alarm(10);

	//표준입력을 read함수를 통해 얻습니다.
	if((n = read(STDIN_FILENO,buf,LINE_MAX))<0){
		fprintf(stderr,"read() error\n");
		exit(1);
	}
	
	// SIGALRM을 프로세스에게 전달시켜줍니다.
	alarm(0);
	//write함수를 통해 표준출력으로 출력합니다.
	write(STDOUT_FILENO,buf,n);
	exit(0);
}

static void ssu_alarm(int signo){
	printf("ssu_alarm() called!\n");
}

