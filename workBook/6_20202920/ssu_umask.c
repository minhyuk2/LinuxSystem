#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define RW_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main (void){

	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	umask(0);//umask값을 0으로 설정한다.

	if(creat(fname1,RW_MODE)<0){
		fprintf(stderr,"creat error for %s\n", fname1);
		exit(1);
	}//fname1의 값인 ssu_file1을 이름으로  가진 파일을 만든다.
	umask(S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	//umasek값을 다시 한 번 변경해준다.

	if(creat(fname2,RW_MODE)<0){
		fprintf(stderr,"creat error for %s\n",fname2);
		exit(1);
	}
	//fname2인 ssu_file2를 이름으로 파일을 생성한다
	exit(0);
}
