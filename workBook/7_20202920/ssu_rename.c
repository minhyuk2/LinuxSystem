#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	
	if(argc!=3){
		fprintf(stderr,"usage: %s <oldname> <newname>\n",argv[0]);
		exit(1);
	}

	if((fd = open(argv[1], O_RDONLY)) < 0 ){
		fprintf(stderr, "first open error for %s\n",argv[1]);
		exit(1);
	}//첫번째 인자로 들어온 파일을 open 해준다.
	else
		close(fd);

	if(rename(argv[1], argv[2]) < 0 ){
		fprintf(stderr, "rename error\n");
		exit(1);
	}
	//첫번째 인자로 들어온 파일의 이름을 두 번째 인자의 값으로 바꿔준다.

	if((fd = open(argv[1],O_RDONLY)) < 0)
		printf("second open for %s\n", argv[1]);
	else {
		fprintf(stderr, "it's very strange!\n");
		exit(1);
	}
	//첫번째 인자 값으로 파일을 열어보고 안열리면 printf로 출력해준다.

	if((fd = open(argv[2],O_RDONLY))<0){
		fprintf(stderr , "third open error for %s\n",argv[2]);
		exit(1);
	}
	//두 번째 인자의 값으로 파일을 open합니다.

	printf("Everything is good!\n");
	exit(0);
}
