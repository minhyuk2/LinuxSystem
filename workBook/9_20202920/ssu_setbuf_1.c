#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void){

	char buf[BUFFER_SIZE];
	
	setbuf(stdout, buf); //setbuf 함수를 통해서 버퍼를  설정해준다.
	printf("Hello, ");
	sleep(1);
	//hello를 출력하고 1초를 쉰다.
	printf("OSLAB!!");
	sleep(1);
	//OSLAB을 출력하고 1초 쉰다.
	printf("\n");
	sleep(1);

	setbuf(stdout,NULL);
	//setbuf에  stdout의 버퍼를 NULL로 설정하여 기존에 설정된 버퍼를 해제한다.
	printf("How");
	sleep(1);
	printf(" are");
	sleep(1);
	printf(" you?");
	sleep(1);
	printf("\n");
	exit(0);
}
