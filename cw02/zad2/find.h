#ifndef find_h
#define find_h

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

// it's common function for callings -atime and -mtime, depth here is optional (-1 when not calling -maxdepth)
int find_time(char *path, char *operation, char sign, int number, int depth);

// classic -max_depth parameter, without -atime or -mtime
int max_depth(char *path, int depth);

#endif