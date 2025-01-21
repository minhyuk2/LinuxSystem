#include <stdio.h>
#include <stdlib.h>

int main(int argc, char*argv[]){
	extern char **environ;
	char **str;
	int i;

	for(i = 0; i < argc; i++)
		printf("argv[%d]: %s\n",i,argv[i]);
	//매개변수로 들어온 값을 다 출력합니다.
	for(str = environ; *str != 0; str++)
		printf("%s\n",*str);
	//environ  문자열을 한줄씩 출력합니다
	exit(0);
}

