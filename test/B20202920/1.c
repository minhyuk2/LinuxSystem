#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>



#define BUFFER_SIZE 7
#define FILE_MAX 256
#define STR_TIME_FORMAT "%y-%m-%d/%H:%M:%S"
#define MAXBUF 20
int main(int argc, char *argv[])
{
	struct sigaction sa;
	struct timespec tspec;
	struct tm tm_now;
	
	ssize_t length;
	size_t sz_buf =32;
	char tmbuf[32];
	char buffer[BUFFER_SIZE];
	char before_file[FILE_MAX];
	char after_file[FILE_MAX];
	int found = 0;
	int fd;
	int fd_before , fd_after;
	char buffering[MAXBUF];
	char buffers[MAXBUF];
	char *pointing;
	if(argc !=3){
		fprintf(stderr,"Usage ./a.out <Inputfile> <token>\n");
		exit(1);
	}

	//make filename
	sprintf(before_file ,"%s_before.txt",argv[1]);
	sprintf(after_file , "%s_after.txt",argv[1]);
	
	//signal handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;
	sa.sa_handler=SIG_IGN;
	sigaction(SIGINT,&sa,NULL);
	

	printf("File split into \'%s\' and \'%s\' at the first occurence of \'%s\'.\n",before_file,after_file,argv[2]);
	printf("The last newline in \'%s\' is preserved.\n",before_file);

	if((fd = open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}

	if((fd_before = open(before_file,O_CREAT|O_RDWR|O_TRUNC,0777))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}
	if((fd_after = open(after_file, O_CREAT|O_RDWR|O_TRUNC, 0777))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}
	char more[MAXBUF];
	while((length = read(fd,buffers,MAXBUF))>0){
		if((pointing=strstr(buffers,argv[2]))!=NULL){
			buffers[MAXBUF] = '\0';
			strcpy(buffering,pointing);
			strncpy(more,buffers,MAXBUF-strlen(buffering));
			more[MAXBUF-strlen(buffering)] = '\0';
			write(fd_before,more,strlen(more));
			write(fd_after,buffering,strlen(buffering));
			found ++;
			break;
		}
		write(fd_before,buffers,length);
	}
	

	//if there is no finding line
	if(found == 0){
		close(fd_after);
		unlink(after_file);
	}

	//write for remain
	while((length = read(fd,buffer,BUFFER_SIZE))>0){
		write(fd_after,buffer,length);
	}
	time_t now;
	time(&now);
	struct tm *tms;
	tms = localtime(&now);
	printf("before Sleep: %02d-%02d-%02d/%02d:%02d:%02d\n",tms->tm_year-100,tms->tm_mon+1,tms->tm_mday,tms->tm_hour,tms->tm_min,tms->tm_sec);
	sleep(10);
	time(&now);
	tms = localtime(&now);
	printf("after Sleep: %02d-%02d-%02d/%02d:%02d:%02d\n",tms->tm_year-100,tms->tm_mon+1,tms->tm_mday,tms->tm_hour,tms->tm_min,tms->tm_sec);
	close(fd);
	close(fd_before);
	close(fd_after);
	exit(0);
}
