#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define DIRECTORY_SIZE MAXNAMLEN

int main(int argc, char *argv[]){
	struct dirent *dentry;
	struct stat statbuf;
	char filename[DIRECTORY_SIZE+1];
	DIR *dirp;

	if (argc < 2){
		fprintf(stderr, "usage: %s <directory>\n",argv[0]);
		exit(1);
	}
	//인자가 2개보다 작으면 오류를 출력한다.

	if((dirp = opendir(argv[1]))==NULL || chdir(argv[1])== -1){
		fprintf(stderr, "opendir, chdir error for %s\n",argv[1]);
		exit(1);
	}
	//dir를 열어주고 파일 이름만 넣어줄 때도 접근할 수 있게끔 작업디렉토리를 변경시켜준다.

	while((dentry = readdir(dirp)) != NULL){
		if( dentry->d_ino == 0)
			continue;
		
		memcpy(filename, dentry->d_name,DIRECTORY_SIZE);
		//메모리 값을 복사해서 넣어준다.

		if(stat(filename, &statbuf) == -1){
			fprintf(stderr, "stat error for %s\n",filename);
			break;
		}//stat구조체를 얻기 위해  stat함수를 수행한다.
		
		if((statbuf.st_mode & S_IFMT) == S_IFREG)
			printf("%-14s %ld\n",filename, statbuf.st_size);
		else
			printf("%-14s\n",filename);
	}
	//mode에 대한 조건을 통해 if문으로 분기한다.

	exit(0);
}
