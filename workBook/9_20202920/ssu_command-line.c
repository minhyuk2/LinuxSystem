#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i;

	for(i=0;i<argc;i++)
		printf("argv[%d] : %s\n", i, argv[i]);
		//함수의 인자로 들어온 것에 대해서 하나씩 구분하여 출력해준다
	exit(0);
}
