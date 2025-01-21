#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr, "usage: %s <actualname> <sysname>\n",argv[0]);
		exit(1);
	}//인자가 3개가 아니면 에러메세지를 리턴한다.


	if(symlink(argv[1],argv[2]) < 0){
		fprintf(stderr,"symlink error\n");
		exit(1);
	}//인자로 들어온 것들을 이용하여 argv[2]를  argv[1]의 심볼릭 링크 파일을 생성한다.
	else
		printf("symlink: %s -> %s\n", argv[2], argv[1]);

	exit(0);
}
