#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i;

	if(argc < 2){
		fprintf(stderr,"usage: %s <file1> <file2> .. <fileN>\n", argv[0]);
		exit(1);
	}//인자가 적은 경우에 에러메세지를 출력한다.

	for(i=1; i<argc;i++){
		if(access(argv[i],F_OK)<0){
			fprintf(stderr,"%s doesn't exist.\n",argv[i]);
			continue;
		}

		if(access(argv[i],R_OK) ==0 )
			printf("User can read %s\n",argv[i]);

		if(access(argv[i],W_OK)==0)
			printf("User can write %s\n",argv[i]);
		if(access(argv[i],X_OK)==0)
			printf("User can execute %s\n",argv[i]);

	}//입력하는 인자의 갯수만큼 반복하여 access 했을 때 파일 접근 권한에 대해서  printf해준다.

	exit(0);
}
