#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/wait.h>

double ssu_maketime(struct timeval *time);

void term_start(int stat);

void ssu_print_child_info(int stat, struct rusage *rusage);

int main(void){
	struct rusage rusage;
	pid_t pid;
	int status;

	if((pid = fork()) == 0){
		char *args[] = {"find", "/", "-maxdepth", "4", "-name", "stdio.h",NULL};

		if(execv("/usr/bin/find",args) < 0 ){
			fprintf(stderr,"execv error\n");
			exit(1);
		}
	}//fork를 통해 자식프로세스를 생성하고 execv를 통해 인자를 넘겨줍니다.


	if(wait3(&status,0,&rusage)==pid)
		//wait3함수를 호출하여 자식 프로세스의 상태를 확인하고  pid와 일치하는 경우에는 아래의 함수를 실행시킵니다.
		ssu_print_child_info(status,&rusage);
	else{
		//예외처리를 해줍니다.
		fprintf(stderr,"wait3 error\n");
		exit(1);
	}
	exit(0);
}

//ssu_maketime함수를 정의해줍니다.
double ssu_maketime(struct timeval *time){
	return ((double)time -> tv_sec + (double)time -> tv_usec/1000000.0);
}

void term_stat(int stat){
	//프로세스가 정상종료된 경우
	if(WIFEXITED(stat))
		printf("normally terminated. exit status = %d\n",WEXITSTATUS(stat));
	//프로세스가 시그널에 의해 종료된 경우
	else if (WIFSIGNALED(stat))
		printf("abnormal termination by signal %d. %s\n", WTERMSIG(stat),
#ifdef WCOREDUMP
				WCOREDUMP(stat)?"core dumped":"no core"
#else
				NULL
#endif
			  );
	//프로세스가 일시적으로 종료된 경우
	else if(WIFSTOPPED(stat))
		printf("stopped by signal %d\n", WSTOPSIG(stat));
}
//자식 프로세스의 종료 정보와 자원 사용 정보를 출력하는 함수
void ssu_print_child_info(int stat, struct rusage *rusage){
	printf("Termination info follows\n");
	term_stat(stat);
	printf("user CPU time : %.2f(sec)\n",
			ssu_maketime(&rusage->ru_utime));
	printf("system CPU time : %.2f(sec)\n",
			ssu_maketime(&rusage->ru_stime));
}
