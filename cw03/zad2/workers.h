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

// structure to hold index of pair matrices which process wants to multiply
// and index of column which the process wants to multiply only
struct Task{
    int pair_index;
    int column_index;
};

// it returns number of digits of number given in argument
int get_digits_number(int number);

// this is the majority function which takes path to the list of matrices and creates processes which will be multiplying these matrices
int manage_process(char *list, int processes_number, int timeout, int mode);

#endif