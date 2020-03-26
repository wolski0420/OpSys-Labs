#ifndef matrix_h
#define matrix_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h> 
#include <linux/limits.h>

struct Matrix{
    int** numbers;
    int rows;
    int cols;
};

int get_col_number(char *path_to_matrix);

int get_row_number(char *path_to_matrix);

struct Matrix load_matrix_from_file(char* filename);

void create_empty_matrix(int rows, int columns, char* filename);

#endif