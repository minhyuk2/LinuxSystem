#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>

int main(int argc, char *argv[]){

	struct utimbuf time_buf;
	struct stat statbuf;
	int fd;
	int i;

	for(i = 1; i < argc; i++){
		if(stat(argv[i], &statbuf) < 0){
			fprintf(stderr, "stat error for %s\n",argv[i]);
			continue;
		}//인자로 들어온 것에 대한 stat구조체를 얻기 위해 stat함수를 사용한다.

		if((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0){
			fprintf(stderr, "open error for %s\n",argv[i]);
			continue;
		}//해당 파일을 open 한다.
		
		close(fd);
		time_buf.actime = statbuf.st_atime;
		time_buf.modtime = statbuf.st_mtime;
		//tiem_buf에 stat구조체의 atime 과 mtime을 대입시킨다.

		if(utime(argv[i], &time_buf) < 0){
			fprintf(stderr, "utime error for %s\n",argv[i]);
			continue;
		}//utime함수를 사용하여 최종 접근 시간과 최정 변경 시간을 변경해준다.

	}
	exit(0);
}
