#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void){
	
	char buf[BUFFER_SIZE];
	int length;
	length = read(0,buf,BUFFER_SIZE);
	//표준입력을 통해서 read를 한다.
	write(1, buf, length);
	//표준출력에 write를 해서 출력을 한다.
	exit(0);
}

