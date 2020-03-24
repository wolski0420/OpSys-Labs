#ifndef list_h
#define list_h

#include <stdio.h>
#include<stdlib.h> 
#include <dirent.h>
#include <sys/stat.h>
#include<sys/wait.h> 
#include<unistd.h> 
#include<string.h> 

// this function creates processes for every directory and "ls -l" them
int scan_directory(char *path);

#endif