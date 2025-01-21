#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	char *argv[] = { "ssu_execl_test_1","param1","param2",(char *)0};

	char *env[] = {
		"NAME = value",
		"nextname=nextvalue",
		"HOME=/home/minhyuk",
		(char*)0
	};
	//argv로 인자를 env로 환경변수를 설정해줍니다.

	printf("this is the original program\n");
	execve("./ssu_execl_test_1",argv,env);
	//execve함수를 실행시키는데 위에서 설정한 인자와 환경변수를 넘겨줍니다.
	printf("%s\n","This line should never get printed\n");
	exit(0);
}

