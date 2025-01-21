#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void ssu_timestamp(char *str);

int main(void){
	unsigned int ret;

	//함수를 호출시켜줍니다.
	ssu_timestamp("before sleep()");
	//10초 기달려줍니다.
	ret = sleep(10);
	ssu_timestamp("after sleep()");
	printf("sleep() returned %d\n",ret);
	exit(0);
}

void ssu_timestamp(char *str){
	time_t time_val;
	//time함수를 수행시켜 시각을 출력시켜줍니다
	time(&time_val);
	printf("%s the time is %s\n",str,ctime(&time_val));
}
