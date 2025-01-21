#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <time.h>

static void ssu_alarm(int signo);
static void ssu_func(int signo);
void ssu_mask(const char *str);

static volatile sig_atomic_t can_jump;
static sigjmp_buf jump_buf;

int main(void)
{
	//SIGUSR1 시그널을 처리하기 위한 ssu_func함수를 등록해줍니다.
	if(signal(SIGUSR1,ssu_func)== SIG_ERR){
		fprintf(stderr,"SIGUSR1 error");
		exit(1);
	}

	//signal을 처리할 함수를 등록해줍니다.
	if(signal(SIGALRM,ssu_alarm) == SIG_ERR){
		fprintf(stderr, "SIGALARM error");
		exit(1);
	}

	ssu_mask("starting main: ");

	//setjump로 점프할 때 올 곳을 설정해줍니다
	if(sigsetjmp(jump_buf,1)){
		ssu_mask("ending main: ");
		exit(0);
	}

	can_jump = 1;

	//while문을 실행시켜줍니다.
	while(1)
		pause();

	exit(0);
}

void ssu_mask(const char *str){
	sigset_t sig_set;
	int err_num;

	err_num = errno;
	 
	//sigprocmask로 블락할 것을 설정해줍니다.
	if(sigprocmask(0,NULL,&sig_set)<0){
		printf("sigprocmask() error");
		exit(1);
	}

	printf("%s",str);

	//sig_set에  SIGINT가 있는 경우 출력시켜줍니다.
	if(sigismember(&sig_set,SIGINT))
		printf("SIGINT ");

	//SIGQUIT가 있는 경우에 출력시켜줍니다.
	if(sigismember(&sig_set,SIGQUIT))
		printf("SIGQUIT ");

	//SIGUSR1가 있는 경우에 출력시켜줍니다.
	if(sigismember(&sig_set,SIGUSR1))
		printf("SIGUSR1 ");
	if(sigismember(&sig_set,SIGALRM))
		printf("SIGALRM ");

	printf("\n");
	errno = err_num;
}


static void ssu_func(int signo){
	time_t start_time;

	//can_jump가 0인 경우 리턴시켜줍니다.
	if(can_jump==0)
		return;

	//함수를 호출시켜줍니다.
	ssu_mask("starting ssu_func: ");
	alarm(3);
	start_time = time(NULL);

	while(1)
		if(time(NULL) > start_time + 5)
				break;
	
	ssu_mask("ending ssu_func: ");
	can_jump = 0;
	//jmp해줍니다.
	siglongjmp(jump_buf,1);

}

static void ssu_alarm(int signo){
	ssu_mask("in ssu_alarm: ");
}

