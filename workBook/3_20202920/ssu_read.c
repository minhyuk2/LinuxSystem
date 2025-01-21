#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "ssu_employee.h"


int main(int argc, char *argv[]){
	struct ssu_employee record;
	int fd;
	int record_num;
	//사용할 변수에 대한 선언	
	if(argc <2){
		fprintf(stderr,"Usage : %s file\n", argv[0]);
		exit(1);
	} //main함수의 매개변수가 2개 이하면 오류가 발생하게끔 예외처리

	if((fd = open(argv[1], O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",argv[1]);
		exit(1);
	} //두번째 인자로 받은 파일에 대해 open 한다.

	while (1){
		printf("Enter reocord number : ");
		scanf("%d", &record_num);
		//record_num을 입력받는다.
		if(record_num <0)
			break;//record_num이 0보다 작은 경우에 대한 예외처리다.
		if(lseek(fd,(long)record_num * sizeof(record),0) <0){
			fprintf(stderr, "lseek error\n");
			exit(1);
		} //입력받은 넘버까지 파일 offset의 위치를 이동시킨다.

		if(read(fd, (char *)&record, sizeof(record))>0)
			printf("Employee : %s Salary : %d\n",record.name,record.salary);
			//읽어서 결과출력
		else
			printf("Record %d not found\n",record_num);
		//파일에서 읽어서 화면에 출력해준다.

	}
	close(fd);
	exit(0);
}





