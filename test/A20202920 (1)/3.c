#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MODE_EXEC (S_IRUSR|S_IWUSR|S_IXUSR|S_IXGRP)

int main(int argc, char *argv[])
{
	
	struct stat statbuf;


	for(int i = 1 ; i<argc; i++){
		
		if(stat(argv[i],&statbuf)<0){
			fprintf(stderr,"stat error\n");
			exit(1);
		}
		
		statbuf.st_mode |= MODE_EXEC;

		if(chmod(argv[i],statbuf.st_mode)<0){
			printf("%s : chmod error\n",argv[i]);
			continue;
		}
		printf("%s : file permission was changed.\n",argv[i]);

	}

	exit(0);
}
