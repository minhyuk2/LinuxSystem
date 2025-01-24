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
	int fd2,fd3,fd4;
	int length =0, offset = 0, count =0;
	int line1= 0, line2=0, line3=0;
	char buf[BUFFER_SIZE];
	char word[WORD_MAX];

	char *tempfile = "temp.txt";
	
	if(argc < 4){
		fprintf(stderr,"argc is less than 4\n");
		exit(1);
	}
	//when argc is less than 4 error cause

	if((fd = open(tempfile,O_WRONLY|O_CREAT|O_TRUNC,0777))<0){
		fprintf(stderr,"open error from %s\n",tempfile);
		exit(1);
	}
	//file open
	if((fd2 = open(argv[1],O_RDONLY))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}

	if((fd3 = open(argv[2],O_RDONLY))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}

	if((fd4 = open(argv[3],O_RDONLY))<0){
		fprintf(stderr,"open error\n");
		exit(1);
	}

	while((length = read(fd2,buf,BUFFER_SIZE))>0){
		for(int i = 0; i < length; i++){
			if(buf[i]=='\n')
				count++;
		}
	}
	line1= count;
	count =0;
	while((length = read(fd3,buf,BUFFER_SIZE))>0){
	         for(int i = 0; i < length; i++){
	            if(buf[i]=='\n')
	                count++;
	        }
	  }
	line2= count;
	count =0;
	while((length = read(fd4,buf,BUFFER_SIZE))>0){
         for(int i = 0; i < length; i++){
		         if(buf[i]=='\n')
	                 count++;
         }
      }
	line3 =count;
	lseek(fd2,0,SEEK_SET);
	lseek(fd3,0,SEEK_SET);
	lseek(fd4,0,SEEK_SET);

	
	offset = line1 + line2 + line3;
	if((offset%3 == 0)||(offset%3 ==2)){
		//bigger is first
		if((line1>line2&&line1>line3)||(line1==line2&&line2==line3&&line1==line3))
		{
			while((length = read(fd2,buf,BUFFER_SIZE))>0){
				write(fd,buf,length);
			}
			if(line2 == line3 || line2>line3){
				while((length =read(fd3,buf,BUFFER_SIZE))>0){
					write(fd,buf,length);
				}
				while((length = read(fd4,buf,BUFFER_SIZE))>0){
					write(fd,buf,length);
				}	
			}
			else{	
				while((length = read(fd4,buf,BUFFER_SIZE))>0){
					write(fd,buf,length);
				}
				while((length = read(fd3,buf,BUFFER_SIZE))>0)
					write(fd,buf,length);
			}
		}
        else if(line2>line1&&line2>line3)
          {
              while((length = read(fd3,buf,BUFFER_SIZE))>0){
                  write(fd,buf,length);
              }
             if(line1== line3 || line1>line3){
                  while((length =read(fd2,buf,BUFFER_SIZE))>0){
                      write(fd,buf,length);
                  }
                  while((length = read(fd4,buf,BUFFER_SIZE))>0){
                      write(fd,buf,length);
                  }
              }
			 else{
				while((length = read(fd4,buf,BUFFER_SIZE))>0)
					write(fd,buf,length);
				while((length = read(fd2,buf,BUFFER_SIZE))>0)
					write(fd,buf,length);
			 }
          }
		else if(line3>line2&&line3>line1)
          {
              while((length = read(fd4,buf,BUFFER_SIZE))>0){
                  write(fd,buf,length);
              }
              if(line1 == line2 || line1>line2){
                  while((length =read(fd2,buf,BUFFER_SIZE))>0){
                      write(fd,buf,length);
                  }
                  while((length = read(fd3,buf,BUFFER_SIZE))>0){
                      write(fd,buf,length);
                  }
             }
			  else{
				  while((length = read(fd3,buf,BUFFER_SIZE))>0){
					  write(fd,buf,length);
				  }
				  while((length = read(fd2,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				}

		  }
    }
	else{

		if((line1 <line2 &&line1 < line3) ||(line1==line2&&line2==line3&&line3==line1)){
			
				  while((length = read(fd2,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				  
				  if(line2 < line3 || line2 ==line3){

					while((length = read(fd3,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
					while((length = read(fd4,buf,BUFFER_SIZE))>0)
						write(fd,buf,length);

				}
				  else{

				  while((length = read(fd4,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);

		
				  while((length = read(fd3,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);

				  
				  }
				  }
		else if(line2 < line1 && line2 < line3){
				

				  while((length = read(fd3,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				if(line1 < line3||line1==line3){
				  while((length = read(fd2,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);

				  while((length = read(fd4,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				}else
				{
					while((length = read(fd4,buf,BUFFER_SIZE))>0)
						write(fd,buf,length);
				  while((length = read(fd2,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				}
		}
		else{	
				  while((length = read(fd4,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);

				if(line1 < line2|| line1== line2){
						
				  while((length = read(fd2,buf,BUFFER_SIZE))>0)
						write(fd,buf,length);
				while((length = read(fd3,buf,BUFFER_SIZE))>0)
						write(fd,buf,length);
				}
				else{

				  while((length = read(fd3,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				  while((length = read(fd2,buf,BUFFER_SIZE))>0)
					  write(fd,buf,length);
				}
		}

	}
	close(fd);
	close(fd2);
	close(fd3);
	close(fd4);

	exit(0);
}
