#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

int main(int argc, char *argv[]){
	struct ssu_employee record;
	int fd;

	if(argc < 2){
		fprintf(stderr, "usage : %s file\n",argv[0]);
		exit(1);
	}
	//매개변수가 2개보다 작으면 에러를 출력하고 프로그램을 종료한다.

	if((fd = open(argv[1],O_WRONLY | O_CREAT | O_EXCL , 0640))<0){
		fprintf(stderr, "open error for %s\n",argv[1]);
		exit(1);
	}
	//두번째 매개변수를 파일명으로 하는 파일을 open한다.
	
	while (1) {
		printf("Enter employee name <SPACE> salary: ");
		scanf("%s",record.name);
		//기록할 이름을 입력받는다.

		if(record.name[0] =='.')
			break;
		//.으로 시작되는 이름을 입력받으면 종료한다.

		scanf("%d", &record.salary);
		//봉급을 입력받는다.
		record.pid = getpid();
		//함수를 호출한 프로세스의 id 를 저장한다.
		write(fd,(char *)&record , sizeof(record));
		//아까 열어둔 파일에 record를 적는다
	}
	close(fd);
	exit(0);
}
