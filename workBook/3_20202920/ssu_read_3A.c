#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	char c;
	int fd;

	if((fd = open("ssu_test.txt", O_RDONLY))<0){
		fprintf(stderr ,"open error for %s\n","ssu_test.txt");
		exit(1);
	}
	//ssu_test.txt파일을 열어준다.
	
	while (1) {
		if ( read(fd,&c,1) > 0)
			putchar(c);
		else
			break;
	}
	//ssu_test.txt 파일에 있는 것들을 한글자씩 화면에 출력한다.
	exit(0);
}
