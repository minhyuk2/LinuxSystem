#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

static void ssu_func(int signo);
void ssu_print_mask(const char *str);

int main(void)
{
	sigset_t new_mask, old_mask, wait_mask;
	ssu_print_mask("program start: ");

	//SIGINT에 대한 핸들러를 설정해줍니다.
	if(signal(SIGINT,ssu_func) == SIG_ERR){
		fprintf(stderr,"signal(SIGINT) error\n");
		exit(1);
	}

	//sigemptyset으로 wait_mask를 초기화하고 addset으로 추가시켜줍니다.
	sigemptyset(&wait_mask);
	sigaddset(&wait_mask,SIGUSR1);
	//sigemptyset으로 new_mask를 초기화하고 addset으로 추가시켜줍니다.
	sigemptyset(&new_mask);
	sigaddset(&new_mask,SIGINT);
	
	//sigprocmask로 블록시켜줍니다.
	if(sigprocmask(SIG_BLOCK,&new_mask,&old_mask)<0){
		 fprintf(stderr,"SIG_BLOCK() error\n");
		 exit(1);
	}

	ssu_print_mask("in critical region: ");

	//시그널이 발생할때 까지 팬딩시킵니다.
	if(sigsuspend(&wait_mask) != -1){
		fprintf(stderr,"sigsuspend() error\n");
		exit(1);
	}


	ssu_print_mask("after return from sigsuspend: ");

	//시그널의 블록을 해제합니다.
	if(sigprocmask(SIG_SETMASK,&old_mask,NULL)<0){
		fprintf(stderr, "SIG_SETMASK() error\n");
		exit(1);
	}

	ssu_print_mask("program exit: ");
	exit(0);
}

void ssu_print_mask(const char *str){
	sigset_t sig_set;
	int err_num;

	err_num = errno;

	//sigprocmask함수를 실행시켜줍니다.
	if(sigprocmask(0,NULL,&sig_set) <0){
		fprintf(stderr,"sigprocmask() error\n");
		exit(1);
	}

	printf("%s",str);

	//SIGINT가 있는지 확인합니다.
	if(sigismember(&sig_set,SIGINT))
		printf("SIGINT ");

	//SIGQUIT가 있는지 확인합니다
	if(sigismember(&sig_set,SIGQUIT))
		printf("SIGQUIT ");
	//SUGUSR1가 있는지 확인합니다.
	if(sigismember(&sig_set,SIGUSR1))
		printf("SIGUSR1 ");
	//SIGALARM이 있는지 확인합니다.
	if(sigismember(&sig_set,SIGALRM))
		printf("SIGALARM ");

	printf("\n");
	errno = err_num;

}
static void ssu_func(int signo){
	ssu_print_mask("\nin ssu_func: ");
}





