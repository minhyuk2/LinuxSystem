#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


#define NAME_SIZE 256


int main(int argc, char *argv[])
{
	FILE *fp;
	char fname[NAME_SIZE];
	struct stat statbuf;

	printf("Enter your file name: ");
	scanf("%s",fname);
	
	if((fp = fopen(fname,"rb"))==NULL){
		fprintf(stderr,"Wrong file. Please check your file name : %s\n",fname);
		exit(1);
	}

	if(lstat(fname,&statbuf)<0){
		fprintf(stderr,"lstat error from %s\n",fname);
		exit(1);
	}
	if(S_ISREG(statbuf.st_mode)){
		printf("File Type: Regular file\n");
	}else if(S_ISDIR(statbuf.st_mode)){
		printf("File Type: Directory\n");
	}else if(S_ISLNK(statbuf.st_mode)){
		printf("File Type: Symbolic link\n");
	}else{
		printf("File Type: Unknown file\n");
	}

	printf("Owner Permission:\n");

	if(access(fname,R_OK)==0){
		printf("Read Permission bit set\n");
	}
	if(access(fname,W_OK)==0){
		printf("Write Permission bit set\n");
	}
	if(access(fname,X_OK)==0){
		printf("Execute Permission bit set\n");
	}

	printf("File Size: %ld bytes\n",statbuf.st_size);

	printf("Last Modificaion Time: %s",ctime(&statbuf.st_mtime));
	fclose(fp);
	exit(0);
}
