#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "ssu_test.txt";
	char *mode = "r";

	if(fopen(fname,mode) == NULL){
		fprintf(stderr, "fopen error for %s\n",fname);
		exit(1);
	}//파일을 오픈시켜준다.
	else 
		printf("Success!\nFilename: <%s>, mode: <%s>\n",fname,mode);
		//열린 파일에 대한 이름과 모드를 출력합니다.
	exit(0);
}
