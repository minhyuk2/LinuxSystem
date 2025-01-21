#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void){
	char buf[BUFFER_SIZE];
	int a, b;
	int i;

	setbuf(stdin,buf);
	//setbuf를 통해 버퍼를 설정한다.
	scanf("%d %d", &a, &b);

	for(i = 0; buf[i] != '\n'; i++)
		putchar(buf[i]);
	//개행문자가 입력될 때까지 입력을 받는다.

	putchar('\n');
	exit(0);
}
