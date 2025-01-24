#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define WORD_MAX 100

int main(int argc, char *argv[])
{
	int fd;
	int length =0, offset = 0, count =0;
	char buf[BUFFER_SIZE];
	char word[WORD_MAX];


	if(argc<2){
		fprintf(stderr,"argc is too small\n");
		exit(1);
	}
	//if argc is less than two -> error cause
	if((fd = open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"open error from %s\n",argv[1]);
		exit(1);
	}
	//open the file
	while((length = read(fd, buf,BUFFER_SIZE))>0){
		for(int i= 0; i <length;i++){
			
			if(buf[i]=='\n'){
				count ++;
				strncpy(word,buf+offset,i-offset);
				printf("%s\n",word);
				offset = i+1;
			}
		}
	}
	//print words one lines
	printf("line number : %d\n",count);	
	//print line number
	close(fd);
	exit(0);
}
