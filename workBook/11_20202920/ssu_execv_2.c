#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	char *argv[] = {"ssu_execl_test_1", "param1", "param2",(char *)0};

	printf("this is the original program\n");
	//execv를 통해서 ssu_execl_test_1을 실행시켜줍니다
	execv("./ssu_execl_test_1",argv);
	//아래의 문장은 따라서 출력되지 않습니다.
	printf("%s\n","This line should never get printed\n");
	exit(0);
}

