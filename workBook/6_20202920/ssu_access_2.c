#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TABLE_SIZE (sizeof(table)/sizeof(*table))

int main(int argc, char *argv[]){
	struct {
		char *text;
		int mode;
	}table[] = {
		{"exists", 0},
		{"execute", 1},
		{"write", 2},
		{"read", 4}
	};//구조체를 선언하고 table이라는 배열을 만들었다.

	int i;

	if(argc < 2){
		fprintf(stderr,"usage : %s <file>\n",argv[0]);
		exit(1);
	}	//인자가 2개보다 작으면 에러 메세지를 출력한다.

	for(i=0;i<TABLE_SIZE;i++){
		if(access(argv[1], table[i].mode) != -1)
			printf("%s -ok\n",table[i].text);
		else
			printf("%s\n",table[i].text);
	}//4번 반복하면서 argv[1]의 접근권하에 대하여 -ok가 붙어져서 출력되게 한다.
	exit(0);
}
