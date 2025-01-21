#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
int main(void){
	char buf[BUFFER_SIZE];
	char *fname = "ssu_test.txt";
	int count;
	int fd1, fd2;

	fd1 = open(fname,O_RDONLY, 0644);
	fd2 = open(fname,O_RDONLY, 0644);
	//만들어진 파일을 각각 열어준다
	if(fd1 < 0 || fd2 < 0){
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	} //둘 중에 하나라도 에러가 발생하면 종료
	count = read(fd1, buf, 25);
	buf[count] = 0; //밑에서 printf하기때문에  Null 문자를 넣어주는 것이다
	printf("fd1's first printf : %s\n",buf);
	lseek(fd1, 1 , SEEK_CUR);//개행문자를 건너뛰기 위해서 사용
	count =  read(fd1, buf, 24);// 24바이트만큼 읽어서 저장
	buf[count] = 0; //위의 buf[count] =0과 동일한 역할을 한다
	printf("fd1's second printf : %s\n",buf);
	count = read(fd2, buf,25);//fd2에서 25 바이트만큼 읽음
	buf[count] = 0;
	//fd1과 같은 과정을 fd2에서 반복
	printf("fd2's first printf : %s\n",buf);
	lseek(fd2, 1, SEEK_CUR);//개행문자 건너뛰기
	count = read(fd2, buf, 24);
	buf[count] = 0;
	printf("fd2's second printf : %s\n",buf);

	exit(0);
}
	
