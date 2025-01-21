#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void ssu_echo_exit(int status);

int main(void)
{
	pid_t pid;
	int status;

	if((pid = fork()) < 0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}//fork를 수행시켜 줍니다.
	else if(pid == 0)
		exit(7);
	//자식 프로세스에서 수행시켜줍니다.

	if(wait(&status) != pid){
		fprintf(stderr, "wait error\n");
		exit(1);
	}//wait를 통해 자식프로세스가 죽고 난 후의 상태를 저장합니다.

	ssu_echo_exit(status);
	//ssu_echo_exit함수를 수행시켜줍니다.

	if((pid = fork()) < 0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}//fork시켜줍니다.
	else if(pid == 0)
		abort();
	//abort함수를 수행시켜줍니다.

	if(wait(&status) != pid){
		fprintf(stderr,"wait error\n");
		exit(1);
	}//wait함수를 수행시켜줍니다.

	ssu_echo_exit(status);
	
	if((pid = fork()) < 0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//fork 시켜줍니다.
	else if(pid == 0)
		status /= 0;


	if(wait(&status) != pid){
		fprintf(stderr, "wait error\n");
		exit(1);
	}//wait함수를 수행시켜줍니다.
	
	ssu_echo_exit(status);
	exit(0);
}

void ssu_echo_exit(int status){
	//ssu_echo_exit함수를 정의해줍니다.
	//매크로들을 통해 상태를 판별합니다.
	if(WIFEXITED(status))
	//자식 프로세스가 정상적으로 종료되었는지 판단합니다.
		printf("normal termination, exit status = %d\n",WEXITSTATUS(status));
	else if(WIFSIGNALED(status))
		printf("abnoraml termination, siganl number = %d%s\n",
				WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? " (core file generated)" : "");
#else 
				"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",WSTOPSIG(status));
}
