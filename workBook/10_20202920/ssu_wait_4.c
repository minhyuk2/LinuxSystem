#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	pid_t child1, child2;
	int pid, status;

	if((child1 = fork()) == 0)
		execlp("date", "date", (char *)0);
	//fork()시켜주고 exec함수를 수행시켜줍니다.
	if((child2 = fork()) == 0)
		execlp("who", "who", (char *)0);
	//두번째 fork시켜줍니다.

	printf("parent: waiting for children\n");

	//자식의 wait을 통해 얻은 status를 수행시켜줍니다.
	while((pid =  wait(&status)) != -1){
		if (child1 == pid)
			printf("parent: first child: %d\n", (status >> 8));
		else if (child2 == pid)

			printf("parent: second child: %d\n",(status >> 8));
	}

	printf("parent: all children terminated\n");
	exit(0);
}

