#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void){
	
	struct stat statbuf;
	char *fname1 = "ssu_file1";
	char *fname2 = "ssu_file2";

	if(stat(fname1,&statbuf) < 0){
		fprintf(stderr, "stat error %s\n",fname1);
	}

	if(chmod(fname1, (statbuf.st_mode & ~S_IXGRP) | S_ISUID) < 0)
		fprintf(stderr, "chmod error %s\n", fname1);
		//fname1의 모드를 변경시켜준다.
	if(chmod(fname2, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IXOTH) < 0 )
		fprintf(stderr, "chmod error %s\n", fname2);
		//fname2의 모드를 변경시켜준다.
	//두 가지의 방식으로 모드를 변경시킬 수 있다는 것을 알려주는 코드이다.
	exit(0);
}
