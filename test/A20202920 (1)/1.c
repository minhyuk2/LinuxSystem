#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>


int main(void){
	char operator;
	int character;
	FILE *fp;
	int number = 0;

	if((fp = fopen("ssu_expr.txt","rb"))==NULL){
		fprintf(stderr,"fopen error\n");
		exit(1);
	}
	
	while(!feof(fp)){
		while((character = fgetc(fp)) !=EOF && isdigit(character))
			number = number *10 + character -48;

		fprintf(stdout," %d\n",number);
		number = 0;

		if(character != EOF){
			ungetc(character, fp);
			operator = fgetc(fp);
			printf("Operator => %c\n",operator);
		}
	}
	fclose(fp);
	exit(0);
}

