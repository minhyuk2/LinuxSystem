#include <stdio.h>
#include <stdlib.h>

void ssu_out(void);

int main(void){
	if(atexit(ssu_out)){
		fprintf(stderr,"atexit error\n");
		exit(1);
	}
	//atexit을 통해 메인 함수가 리턴한 후 실행할 함수를 등록한다.
	exit(0);
}

void ssu_out(void){
	printf("atexit succeeded!\n");
	//exit함수가 실행되고 나서 실행될 함수를 만들어준 것이다.
}
