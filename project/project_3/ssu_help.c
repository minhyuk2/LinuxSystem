#include <stdio.h>

//help와 관련된 명령어들을 정리해놓았습니다.
void printUsage(){
    printf("Usage: ");
}

void printAdd(){
    printf("add <PATH> [OPTION]... : add new daemon process of <PATH> if <PATH> is file\n");
    printf("  -d : add new daemon process of <PATH> if <PATH> is directory\n");
    printf("  -r : add new daemon process of <PATH> recursive if <PATH> is directory\n");
    printf("  -t <TIME> : set daemon process time to <TIME> sec (default : 1sec)\n");
}

void printRemove(){
    printf("remove <DAEMON_PID> : delete daemon process with <DAEMON_PID>\n");
}

void printList(){
    printf("list [DAEMON_PID] : show daemon process list or dir tree\n");
}

void printHelp(){
    printf("help [COMMAND] : show commands for program\n");
}

void printExit(){
    printf("exit : exit program\n");
}

void printAll(){
    printUsage();
    printf("\n");
    printf(" > ");
    printAdd();
    printf(" > ");
    printRemove();
    printf(" > ");
    printList();
    printf(" > ");
    printHelp();
    printf(" > ");
    printExit();
}