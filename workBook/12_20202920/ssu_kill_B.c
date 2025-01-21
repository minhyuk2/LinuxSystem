#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	//5초 sleep하면서 OSLAB을 계속해서 출력합니다.
	while(1){
		printf("\n[OSLAB]");
		sleep(5);
	}

	exit(0);
}

