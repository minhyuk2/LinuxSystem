#define OPENSSL_API_COMPAT 0x10100000L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <wait.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <openssl/md5.h>
#include <stdbool.h>



#ifndef HEADERING
#define HEADERING

#define true 1
#define false 0

#define HASH_MD5  33

#define NAMEMAX 255
#define PATHMAX 4096
#define STRMAX 4096
#define BUF_MAX 4096
#define FILE_MAX 256

#define CMD_ADD			0b0000001 //1
#define CMD_REM			0b0000010 //2
#define CMD_LIST	  0b0000100 //4
#define CMD_HELP		0b0010000
#define CMD_EXIT		0b0100000
#define NOT_CMD			0b0000000

#define OPT_D		0b00001 //1
#define OPT_R	0b00010 //2
#define OPT_T		0b00100 //4
#define NOT_OPT 	0b00000
 
#define defaults 1


//전역변수로 선언되는 것들입니다.
extern char exeNAME[PATHMAX];
extern char exePATH[PATHMAX];
extern char homePATH[PATHMAX];
extern char backupPATH[PATHMAX];
extern char logPATH[PATHMAX];
extern char *commanddata[5];
  


//구조체들입니다.
//exec해줄 때 넘겨주는 인자에 대한 구조체입니다.
typedef struct command_parameter {
 	char *command;
	char *filename;
	char *tmpname;
  int commandopt;
 	char *argv[10];
} command_parameter;

//path를 구해줄 때 쓰기 위한 구조체이다.
typedef struct pathList_ {
  struct pathList_ *next;
  struct pathList_ *prev;
  char path[NAMEMAX];
} pathList;

//각 로그에 적혀 있는 파일에서 어떤 작업들이 이루어졌는지 알려주기 위해서 생성한 구조체입니다.
typedef struct _backupNode {
  struct _dirNode *root_version_dir;
  struct _fileNode *root_file;

  char backup_time[20];
  char action[10];
  char origin_path[PATHMAX];
  char backup_path[PATHMAX];

  struct _backupNode *prev_backup;
  struct _backupNode *next_backup;
} backupNode;


//list명령어에서 사용하기 위해 필요한 file구조체입니다.
typedef struct _fileNode {
  struct _dirNode *root_dir;

  int backup_cnt;
  char backup_time[21];
  char file_name[FILE_MAX];
  char file_path[PATHMAX];
  char made_path[PATHMAX];
  int alive;
  time_t firstMtime;
  backupNode *backup_head;

  struct _fileNode *prev_file;
  struct _fileNode *next_file;
} fileNode;


//list명령어를 사용하기 위해서 필요한 dir구조체입니다.
typedef struct _dirNode {
  struct _dirNode *root_dir;

  int file_cnt;
  int subdir_cnt;
  int backup_cnt;
  char backup_time[21];
  char dir_name[FILE_MAX];
  char dir_path[PATHMAX];
  char parent_path[PATHMAX];
  fileNode *file_head;
  struct _dirNode *subdir_head;

  struct _dirNode* prev_dir;
  struct _dirNode *next_dir;
} dirNode;


//queue를 만들기 위한 구조체다.
typedef struct queue{
   struct queue *first;
	 struct queue *last;
   char path[PATHMAX];
   struct queue *next;
   
 }queue;



//ssu_header.c에 있는 함수원형들입니다.
int realpath_2_relative(char *realpath, char *returnPath);
int ConvertPath(char* origin, char* resolved);
char **GetSubstring(char *str, int *cnt, char *del);
char *Tokenize(char *str, char *del);
int md5(char *target_path, char *hash_result);
int ssu_daemon_init(char*, char*,unsigned int);
char *GetFileName(char file_path[]);
char *makeDate();
char *dateForm(char*);
void sigusr_handler(int signum);
int deleteDirectory(const char* dirname);
char *substr(char *str, int beg, int end);
int ssu_daemon_init_dir(char* currentPath,char *dirName,unsigned int time);
char *getParentPath(char *path);
void enqueue(queue* Queue, char* dirPath);
char*  dequeue(queue* Queue);
int ssu_daemon_init_dirR(char* currentPath,char *dirName,unsigned int time);

//help.c에 있는 함수원형들 입니다.
void printUsage();
void printAdd();
void printRemove();
void printList();
void printHelp();
void printExit();
void printAll();


//ssu_init.c에 있는 함수원형들입니다.
void Init();


//ssu_struct.c에 있는 함수원형들입니다.
int init_backup_list(int log_fd,char* getPath);
void print_list(dirNode* dirList, int depth, int last_bit);
void print_depth(int depth, int is_last_bit);
dirNode *dirNode_insert(dirNode* dir_head, char* dir_name, char* dir_path,char*backup_time);
void dirNode_init(dirNode *dir_node);
fileNode *fileNode_insert(fileNode *file_head, char *file_name, char *dir_path,char *backup_time);
int backup_list_insert(dirNode* dirList, char* backup_time, char* path, char* backup_path);
void fileNode_init(fileNode *file_node);
void fileNode_remove(fileNode *file_node);
fileNode *fileNode_insert_forD(fileNode *file_head, char *file_path);
fileNode *fileNode_get(fileNode *file_head, char *file_path);
dirNode *dirNode_get(dirNode* dir_head, char* dir_path);
void dirNode_remove(dirNode *dir_node);
//BFS를 사용하기 위해서 재귀적으로 반복하여 모니터링하는 함수입니다.
int backupRecursive(queue* Queue,char *path,dirNode *dir_head,char *backupLog,pid_t currentPid);
dirNode *dirNode_insert_forR(dirNode* dir_head,fileNode *fileHead, char* dir_path,char *parentPath);
#endif
