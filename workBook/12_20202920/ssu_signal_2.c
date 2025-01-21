#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//함수원형을 선언해줍니다.
static void ssu_signal_handler(int signo);

int main(void)
{
	//SIGINT에 대한 처리를 ssu_signal_handler로 하고 그때의 리턴이 SIG_ERR인 경우에 대한 예외처리를 합니다
	if(signal(SIGINT,ssu_signal_handler) == SIG_ERR){
		fprintf(stderr,"cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}
	//SIGTERM에 대해 위해서 한 과정과 동일하게 수행해줍니다.
	if(signal(SIGTERM, ssu_signal_handler) == SIG_ERR){
		fprintf(stderr,"cannot handle SIGTERM\n");
		exit(EXIT_FAILURE);
	}
	
	//SIGPROF에 대해서는 기본처리동작으로 처리해줍니다.
	if(signal(SIGPROF,SIG_DFL) == SIG_ERR){
		fprintf(stderr, "cannot reset SIGPROF\n");
		exit(EXIT_FAILURE);
	}

	//SIGPRUP에 대해서는 SIG_IGN으로 처리해줍니다.
	if(signal(SIGHUP, SIG_IGN) == SIG_ERR) {
		fprintf(stderr, "cannot ignore SIGHUP\n");
		exit(EXIT_FAILURE);
	}

	//무한반복하면서 시그널을 기다립니다.
	while(1)
		pause();

	exit(0);
}

static void ssu_signal_handler(int signo){
	if(signo == SIGINT)
		printf("caught SIGINT\n");
	else if(signo == SIGTERM)
		printf("caught SIGTERM\n");
	else{
		fprintf(stderr, "unexpected signal\n");
		exit(EXIT_FAILURE);
	}
	//각각의 시그널이 들어왔을 때에 대한 출력문을 설정해줍니다.

	exit(EXIT_SUCCESS);
}
