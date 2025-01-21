#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void){
	struct stat statbuf;
	char *fname = "ssu_myfile";
	int fd;

	if((fd = open(fname, O_RDWR | O_CREAT, 0600)) < 0){
		fprintf(stderr , "open error for %s\n", fname);
		exit(1);
	}
	//fname을 이름으로 갖는 파일을 없으면 생성하고 있는 경우 RDWR모드로 열어줍니다

	close(fd);
	stat(fname, &statbuf);
	printf("# 1st stat call # UID:%d  GID:%d\n",statbuf.st_uid,statbuf.st_gid);

	if(chown(fname,501,300) < 0){
		fprintf(stderr , "chown error for %s\n",fname);
		exit(1);
	}
	//파일 사용자 id와 그룹 사용자 id를 변경해줍니다.

	stat(fname,&statbuf);
	printf("# 2nd stat call # UID:%d    GID:%d\n",statbuf.st_uid,statbuf.st_gid);

	if(unlink(fname) < 0){
		fprintf(stderr, "unlink error for %s\n",fname);
		exit(1);
	}
	//기존 디렉토리 항목의 링크를 제거해준다.

	exit(0);
}
	
