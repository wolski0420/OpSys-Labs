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

// structure to holds matrix values and size
struct Matrix{
    int** numbers;
    int rows;
    int cols;
};

// this function takes path to file with matrix and returns the number of columns
int get_col_number(char *path_to_matrix);

// this function takes path to file with matrix and returns the number of rows
int get_row_number(char *path_to_matrix);

// this function loads matrix from file to structure
struct Matrix load_matrix_from_file(char* filename);

// this function creates zeros matrix in file
void create_empty_matrix(int rows, int columns, char* filename);

// it multiplies matrices and returns the result matrix
struct Matrix multiply_matrices(struct Matrix first, struct Matrix second);

// it creates a matrix in file with random values
void create_rand_matrix_in_file(int rows, int columns, char *filename);

#endif