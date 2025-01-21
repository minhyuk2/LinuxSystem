#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define EXIT_CODE 1

int main(void)
{
	pid_t pid;
	int ret_val, status;
	//변수를 선언해줍니다.

	if((pid = fork()) == 0){
		printf("childe: pid = %d ppid = %d exit_code = %d\n",
				getpid(),getppid(),EXIT_CODE);
			exit(EXIT_CODE);
	}//fork를 생성시켜주고 자식프로세스의 경우 출력시켜줍니다.

	printf("parent: waiting for child = %d\n",pid);
	ret_val = wait(&status);
	//성공한 프로세스ID를 저장시켜줍니다.
	printf("parent: return value = %d, ", ret_val);
	printf(" child's status = %x",status);
	printf(" and shifted = %x\n", (status >> 8));
	//printf로 출력시켜줍니다.
	exit(0);
}

