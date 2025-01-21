#include <stdio.h>

void printUsage(void){
	printf("Usage:\n");
}

void printBackup(void){

	printf(" > backup <PATH> [OPTION]... : backup file if <PATH> is file\n");
	printf("   -d : backup files in directory if <PATH> is directory\n");
	printf("   -r : backup files in directory recursive if <PATH> is directory\n");
	printf("   -y : backup file although already backuped\n");

}
void printRemove(void){

	printf(" > remove <PATH> [OPTION]... : remove backuped file if <PATH> is file\n");
	printf("   -d : remove backuped files in directory if <PATH> is directory\n");
	printf("   -r : remove backuped files in directory recursive if <PATH> is directory\n");
	printf("   -a : remove all backuped files\n");
}
void printRecover(void){

	printf(" > recover <PATH> [OPTION]... : recover backuped file if <PATH> is file\n");
	printf("   -d : recover backuped files in directory if <PATH> is directory\n");
	printf("   -r : recover backuped files in directory recursive if <PATH> is directory\n");
	printf("   -l : recover lates backuped file\n");
	printf("   -n <NEW_PATH> : recover backuped file with new path\n");
			}

void printList(void){

	printf(" > list [PATH] : show backup list by directory structure is file directory is file directory \n");
	printf("   >> rm <INDEX> [OPTION]... : remove backuped files of [INDEX] with [OPTION]\n");
	printf("   >> rc <INDEX> [OPTION]... : recover backuped files of [INDEX] with [OPTION]\n");
	printf("   >> vi(m) <INDEX> : edit original file of [INDEX]\n");

	printf("   >> exit : exit program\n");
}
void printHelp(void){

	printf(" > help [COMMAND] : show commands for program\n");
}

