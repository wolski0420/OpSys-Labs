#ifndef workers_h
#define workers_h

#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>

int manage_process(char *list, int processes_number, int timeout, int mode);

#endif