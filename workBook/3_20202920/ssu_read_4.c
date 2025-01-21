#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define TABLE_SIZE 128
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){

	static struct {
		long offset;
		int length;
	}table [TABLE_SIZE];

	char buf[BUFFER_SIZE];
	long offset;
	int entry;
	int i;
	int length;
	int fd;

	if(argc < 2){
		fprintf(stderr, "usage:%s <file>\n",argv[0]);
		exit(1);
	}//main함수의 매개변수의 갯수가 2개 보다 작으면 에러를 출력

	if((fd = open(argv[1], O_RDONLY)) <0){
		fprintf(stderr, "open error for %s\n",argv[1]);
		exit(1);
	}//두번째 인자로 들어온 것을 파일의 이름으로 하여 open한다

	entry =0;
	offset=0;
	while((length = read(fd,buf,BUFFER_SIZE))>0){
		for(i = 0 ; i < length; i++){
			table[entry].length++;
			offset++;

			if(buf[i] == '\n')
				table[++entry].offset = offset;
		}
	}
	//개행문자가 나오면 entry를 증가시키고 지금까지의 offset을 저장한다 또한 그 전까지는 각 entry의 length를 증가시킨다.
#ifdef DEBUG
	for(i = 0; i<entry;i++)
		printf("%d : %ld, %d\n", i+1, table[i].offset, table[i].length);
	//DEBUG가 정의된 경우에는 테이블 offset 과 길이를 각각 출력한다.

#endif
	
		while(1){
			printf("Enter line number : ");
			scanf("%d",&length);
			//length 에 값을 저장한다.

			if(--length<0)
				break;
			//길이가 1이하면 종료
			lseek(fd,table[length].offset,0);
			//처음부터 테이블의 offset만큼 파일 오프셋 위치를 이동시킨다.

			if(read(fd,buf,table[length].length)<=0)
				continue;
			buf[table[length].length] = '\0';
			// buf를 주어진 조건에 끊어버린다.
			printf("%s",buf);
		}
		close(fd);
		exit(0);
}

