#include <stdio.h>

void printAll(){
    printf("Usage:\n");
    printf("> add <PATH> : record path to staging area, path will tracking modification\n");
    printf("> remove <PATH> : record path to staging area, path will not tracking modification");
    printf("> status : show staging area status\n");
    printf("> commit <NAME> : backup staging area with commit name\n");
    printf("> revert <NAME> : recover commit directory with commit name\n");
    printf("> log : show commit log\n");
    printf("> help : show commands for program\n");
    printf("> exit : exit program\n");
}
void printAdd(){
    printf("Usage: > add <PATH> : record path to staging area, path will tracking modification \n");
}

void printRemove(){
    printf("Usage: > remove <PATH> : record path to staging area, path will not tracking modification\n");
}

void printStatus(){
    printf("Usage: > status : show staging area status");
}

void printCommit(){
     printf("Usage: > commit <NAME> : backup staging area with commit name\n");
}

void printRevert(){
     printf("Usage: > revert <NAME> : recover commit directory with commit name\n");
}

void printLog(){
     printf("Usage: > log : show commit log\n");
}

void printHelp(){
      printf("Usage: >help : show commands for program\n");
}

void printExit(){
     printf("Usage: > exit : exit program\n");
}
