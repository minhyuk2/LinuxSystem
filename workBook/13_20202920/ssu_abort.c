#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	printf("abort terminate this program\n");
	//SIGABRT시그널을 프로세스 자신에게 보냅니다.
	abort();
	printf("this line is never reached\n");
	exit(0);
}
