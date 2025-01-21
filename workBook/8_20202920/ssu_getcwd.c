#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX  1024

int main(void)
{
	char *pathname;

	if(chdir("/home/minhyuk") < 0){
		fprintf(stderr,"chdir error\n");
		exit(1);
	}
	//현재 작업디렉토리를 변경시켜줍니다.

	pathname = malloc(PATH_MAX);

	if(getcwd(pathname, PATH_MAX) == NULL){
		fprintf(stderr ,"getcwd error\n");
		exit(1);
	}//현재 작업 중인 디렉토리를 확인합니다.

	printf("current directory = %s\n", pathname);
	exit(0);
}
