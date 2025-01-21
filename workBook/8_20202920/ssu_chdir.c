#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	if(chdir("/etc") < 0){
		fprintf(stderr, "chdir error\n");
		exit(1);
	}//현재 작업 디렉토리를 변경시켜줍니다.
	printf("chdir to /etc succeeded.\n");
	exit(0);
}
