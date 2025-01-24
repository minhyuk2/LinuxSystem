#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#define TABLE_SIZE 128
#define BUFFER_SIZE 1024


int main(int argc, char *argv[]){

	static struct{
		long offset;
		int length;
	} table[TABLE_SIZE];
	char buf[BUFFER_SIZE];
	int length;
	int fd;
	int offset=0;
	int line=0;
	int inputnumber;

	if(argc < 2 ){
		fprintf(stderr,"usage: %s <file>\n",argv[0]);
		exit(1);
	}

	if((fd = open(argv[1],O_RDONLY))<0){
		fprintf(stderr, "open error for %s\n",argv[1]);
		exit(1);
	}

	while((length = read(fd , buf ,BUFFER_SIZE))>0){
		for(int i = 0; i<length; i++){
			offset++;
			table[line].length++;
			if(buf[i] == '\n'){
				table[++line].offset = offset;
			}
		}
	}



	while(1){
		printf("Enter line number : ");
		scanf("%d",&inputnumber);
		getchar();

		if(--inputnumber<0){
			break;
		}
		
		if(lseek(fd,table[inputnumber].offset,SEEK_SET)<0){
			fprintf(stderr,"lseek error\n");
			exit(1);
		}

		if(read(fd,buf,table[inputnumber].length)< 0){
			fprintf(stderr,"read error\n");
			exit(1);
		}

		buf[table[inputnumber].length] = '\0';
		printf("%s",buf);
		}
	close(fd);
	exit(0);
}

