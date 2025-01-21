#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
	char *fname = "ssu_test.txt";
	int fd;

	printf("First printf : Hello, OSLAB!!\n");

	if((fd = open(fname, O_RDONLY)) < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}//파일을 RDONLY로 열어준다.


	if(freopen(fname,"w", stdout) != NULL)
		printf("Second printf : Hello, OSLAB!!\n");
	//ssu_test.txt 파일로 stdout을 쓰기 모드로 다시 오픈하여 써준다.

	exit(0);
}
