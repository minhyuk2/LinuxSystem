#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char character;
	int fd;
	int line_count = 0;

	if((fd = open("ssu_test.txt",O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n","ssu_test.txt");
		exit(1);
	} //ssu_test.txt 파일을 열음

	while (1){
		if(read(fd, &character, 1) > 0){
			if(character == '\n')
					line_count++;
				}
			else
				break;
	}//한글자씩 읽는데 개행문자가 나오면 line_count를 하나씩 증가시킨다.
		
	
	printf("Total line : %d\n",line_count);
	exit(0);
}
