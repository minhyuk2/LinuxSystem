#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif

#define MAX_PATH_GUESSED 1024

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

char *pathname;
char command[LINE_MAX], grep_cmd[LINE_MAX];

int ssu_do_grep(void){
	struct dirent *dirp;
	struct stat statbuf;
	char *ptr;
	DIR *dp;

	if(lstat(pathname, &statbuf) < 0 ){
		fprintf(stderr, "lstat error for %s\n",pathname);
		return 0;
	
	}//pathname에 대한 stat구조체를 얻기 위해서 lstat를 사용한다.

	if(S_ISDIR(statbuf.st_mode) == 0){
		sprintf(command, "%s %s", grep_cmd, pathname);
		printf("%s : \n",pathname);
		system(command);
		return 0;
	}//디렉토리가 아닌 경우에 대한 예외처리를 작성해준다.
	
	ptr = pathname + strlen(pathname);

	*ptr++ = '/';
	*ptr = '\0';
	//ptr뒤에 /과 null을 붙혀준다.

	if((dp = opendir(pathname)) == NULL){
		fprintf(stderr, "opendir error for %s\n",pathname);
		return 0;
	}
	//dir를 열어준다.
	while((dirp = readdir(dp)) != NULL)
		if(strcmp(dirp->d_name,".")&& strcmp(dirp->d_name,"..")){
			strcpy(ptr,dirp->d_name);
		//dir 이름이 ..과 ..이 아닌 경우에 ptr에 이름을 넣어준다. 그 후에 ssu_do_grep함수를 사용해준다.
			if(ssu_do_grep() < 0)
				break;
		}
	ptr[-1] = 0;
	closedir(dp);
	return 0;
}

void ssu_make_grep(int argc, char *argv[]){
	int i;
	strcpy(grep_cmd, " grep");
	//grep_cmd에 grep이라는 문자열을 복사해서 넣어준다.
	for(i = 1; i < argc-1; i++){
		strcat(grep_cmd, " ");
		strcat(grep_cmd, argv[i]);
	}
}

int main(int argc, char *argv[]){
	if(argc < 2){
		fprintf(stderr, "usage: %s <-CVbchilnsvwx> <-num> <-A num> <-B num> <-f file>\n"
				"            <-e> expr <directory>\n", argv[0]);
		exit(1);
	}//인자가 2개보다 작으면 에러메세지를 출력한다.

	if(pathmax == 0){
		if((pathmax = pathconf("/", _PC_PATH_MAX)) < 0)
			pathmax = MAX_PATH_GUESSED;
		else
			pathmax++;
	}

	if((pathname = (char *) malloc(pathmax +1)) ==NULL){
		fprintf(stderr, "malloc error\n");
		exit(1);
	} //pathname에 pathmax보다 크게 동적할당을 해준다.

	strcpy(pathname, argv[argc-1]);
	//pathname을 넣어준다.
	ssu_make_grep(argc,argv);
	//ssu_make_grep함수를 실행한다.
	ssu_do_grep();
	//ssu_do_grep함수를 실행한다.
	exit(0);
}
