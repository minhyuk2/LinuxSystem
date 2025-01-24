#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define STUDENT_SIZE 24
#define SCORE_SIZE 3
#define SCORE_OFFSET 10

int main(int argc, char*argv[])
{
	struct flock lock;
	off_t offset = 0;
	char buffer[STUDENT_SIZE + 1];
	char score[SCORE_SIZE +1];
	int fd;
	int student_found =0;
	int subject_index;

	if(argc != 4 ){
		fprintf(stderr,"argc error\n");
		exit(1);
	}


	if((fd = open("students.dat",O_RDWR|O_CREAT,0777))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}
	int length =0;
	while((length = read(fd,buffer,STUDENT_SIZE))>0){
		buffer[STUDENT_SIZE] = '\0';

		if(strstr(buffer,argv[2])!=NULL){
			student_found = 1;
			break;
		}
		offset += length;
	
	}

	if(student_found == 0){
		printf("Ther is no ID\n");
		close(fd);
		exit(0);
	}
	

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start =offset;
	lock.l_len = STUDENT_SIZE;
	if(fcntl(fd,F_SETLK,&lock)==-1){
		printf("Error locking field: Resource temporarily unavailable.\n");
		printf("The field is currently being modified by another proess.\n");
		close(fd);
		exit(1);
	}
	if(!strcmp(argv[1],"korean")){
		memmove(buffer+10,argv[3],3);
	}else if(!strcmp(argv[1],"english")){
		memmove(buffer+15,argv[3],3);
	}else if(!strcmp(argv[1],"math")){
		memmove(buffer+20,argv[3],3);
	}else{
		printf("error\n");
		close(fd);
		exit(1);
	}


	
	printf("Sleeping for 10 seconds to test locking...\n");
	sleep(10);
	close(fd);
	exit(0);

}
