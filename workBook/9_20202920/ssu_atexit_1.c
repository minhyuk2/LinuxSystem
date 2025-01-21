#include <stdio.h>
#include <stdlib.h>

static void ssu_exit1(void);
static void ssu_exit2(void);

int main(void){

	if(atexit(ssu_exit2) != 0){
		fprintf(stderr, "atexit error for ssu_exit2");
		exit(1);
	}
	//atexit으로 ssu_exit2를 설정해줬다.

	if(atexit(ssu_exit1) != 0){
		fprintf(stderr, "atexit error for ssu_exit1");
		exit(1);
	}

	//다시 한 번 atexit으로 ssu_exit1을 exit한 후에 실행될 함수로 설정해줬다.

	if(atexit(ssu_exit1) != 0){
		fprintf(stderr, "atexit error for ssu_exit1");
		exit(1);
	}
	//같은 과정을 한 번 더 반복

	printf("done\n");
	exit(0);

}

static void ssu_exit1(void){
	printf("ssu_exit1 handler\n");
}

static void ssu_exit2(void){
	printf("ssu_exit2 handler\n");
}

