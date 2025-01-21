#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr, "usage: %s <oldname> <newname>\n",argv[0]);
		exit(1);
	}
	
	if(link(argv[1],argv[2]) < 0 ){
		fprintf(stderr, "link error\n");
		exit(1);
	}//인자로 들어온 두 개의 파일을 link시켜준다.
	else
		printf("step1 passed.\n");

	if(remove(argv[1])<0){
		fprintf(stderr,"remove error\n");
		remove(argv[2]);
		exit(1);
		
	}//첫번째 인자로 들어온 파일이 remove에 실패한 경우 두번째 인자를 remove해준다.
	else
		printf("step2 passed.\n");

	printf("Success!\n");
	exit(0);
}
