#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp, char *buf);

int main(void){
	char buf[BUFFER_SIZE];
	char *fname = "/dev/pts/0";
	FILE *fp;

	if((fp = fopen(fname,"w")) == NULL){
		fprintf(stderr, "fopen error for %s", fname);
		exit(1);
	}//읽기 모드로 파일을 열어준다.


	ssu_setbuf(fp,buf);
	fprintf(fp,"Hello, ");
	//파일에 Hello 를 적어준다.
	sleep(1);
	fprintf(fp,"UNIX!!");
	sleep(1);
	fprintf(fp,"\n");
	sleep(1);
	ssu_setbuf(fp,NULL);
	//ssu_setbuf함수를 호출해준다.
	fprintf(fp,"HOW");
	sleep(1);
	fprintf(fp," ARE");
	sleep(1);
	fprintf(fp," YOU?");
	sleep(1);
	fprintf(fp, "\n");
	sleep(1);
	exit(0);
}

void ssu_setbuf(FILE *fp, char *buf){
	size_t size;
	int fd;
	int mode;
	
	fd = fileno(fp);
	//fileno함수를 통해 파일 디스크립터 값을 fd에 저장한다.
	if(isatty(fd))
		mode = _IOLBF;
	//isatty함수를 통해 파일 디스크립터가 터미널 디바이스인지 확인한다.
	else
		mode - _IOFBF;

	if(buf == NULL){
		mode = _IONBF;
		size = 0;
	}
	else
		size = BUFFER_SIZE;
	//buf의 상태에 따라 if문을 분기해준다
	setvbuf(fp,buf,mode,size);
	//setvbuf함수를 사용한다.
}
