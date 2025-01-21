#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if(argc < 3 ) {
		fprintf(stderr , "usage: %s <file1> <file2>\n",argv[0]);
		exit(1);
	}
	
	if(link(argv[1],argv[2]) == -1){
		fprintf(stderr, "link error for %s\n",argv[1]);
		exit(1);
	}
	//인자로 입력된 두 개의 파일에 링크를 생성해준다.

	exit(0);
}
