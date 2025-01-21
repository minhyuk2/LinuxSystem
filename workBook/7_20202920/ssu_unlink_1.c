#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char *fname = "ssu_dump.txt";

	if(open(fname, O_RDWR) < 0) {
		fprintf(stderr, "open error for %s\n",fname);
		exit(1);
	}
	//파일을 열어줍니다.

	if(unlink(fname) <0 ){
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}
	//링크를 제거해줍니다.

	printf("File unlinked\n");
	sleep(20);
	printf("Done\n");
	//sleep함수를 통해 기다려준 후 Done을 출력합니다.
	exit(0);
}
