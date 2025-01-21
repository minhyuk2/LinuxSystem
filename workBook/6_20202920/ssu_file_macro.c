#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
	struct stat file_info;
	char *str;
	int i;

	for(i = 1; i<argc; i++){
		printf("name = %s, type = ",argv[i]);


		if(lstat(argv[i], &file_info)<0){
			fprintf(stderr,"lstat error\n");
			continue;
		} //lstat에러가 발생하면 메세지를 출력하고 for문을 한 번 건너뛴다.
		
		if(S_ISREG(file_info.st_mode))
			str="regular";
		else if(S_ISDIR(file_info.st_mode))
			str="directory";
		else if(S_ISCHR(file_info.st_mode))
			str="character special";
		else if(S_ISBLK(file_info.st_mode))
			str="block special";
		else if(S_ISFIFO(file_info.st_mode))
			str="FIFO";
		else if(S_ISLNK(file_info.st_mode))
			str="symbolic link";
		else if(S_ISSOCK(file_info.st_mode))
			str="socket";
		else
			str="unknown mode";
	}// 인자로 받은  name을 출력한다.file_info의 st_mode에 따라 다른 type을 출력한다.
	exit(0);
	
}
