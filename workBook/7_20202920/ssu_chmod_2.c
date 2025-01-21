#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define MODE_EXEC (S_IXUSR|S_IXGRP|S_IXOTH)

int main(int argc, char* argv[]){

	struct stat statbuf;
	int i;

	if(argc < 2){
		fprintf(stderr, "usage : %s <file1> <file2>...<fileN>\n",argv[0]);
		exit(1);
	} //인자가 적게 들어왔을 때 에러메세지를 출력하며 프로그램을 종료하는 것이다.

	for(i = 1; i < argc ; i++){
		if(stat(argv[i], &statbuf) < 0){
			fprintf(stderr , "%s : stat error\n",argv[i]);
			continue;
		}
		statbuf.st_mode |= MODE_EXEC;
		statbuf.st_mode ^= (S_IXGRP|S_IXOTH);
		if (chmod(argv[i], statbuf.st_mode) < 0)
			fprintf(stderr, "%s : chmod error\n",argv[i]);
		else
			printf("%s : file permission was changed.\n",argv[i]);
	}//반복문을 통해 인자로 들어온 파일들의 mode를 변경시켜 줍니다.
	exit(0);
}
