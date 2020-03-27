#include "matrix.h"

// this function takes path to file with matrix and returns the number of columns
int get_col_number(char *path_to_matrix){
    FILE *matrix = fopen(path_to_matrix,"r");

    //getting first line
    size_t length = 0;
    char *line = NULL;
    getline(&line,&length,matrix);

    // counting columns
    int counter  = 0;
    while((line = strchr(line,' ')) != NULL){
        counter++;
        line++;
    }

    fclose(matrix);

    return counter+1;
}

// this function takes path to file with matrix and returns the number of rows
int get_row_number(char *path_to_matrix){
    FILE *matrix = fopen(path_to_matrix,"r");

    // getting lines = counting rows
    int rows_number = 0;
    size_t length = 0;
    char *line = NULL;
    while((getline(&line,&length,matrix)) != EOF){
        rows_number++;
    }

    fclose(matrix);

    return rows_number;
}

// this function loads matrix from file to structure
struct Matrix load_matrix_from_file(char *path_to_matrix){
    struct Matrix matrix_to_return;

    // reading size of matrix
    matrix_to_return.cols = get_col_number(path_to_matrix);
    matrix_to_return.rows = get_row_number(path_to_matrix);

    // if we can read matrix
    if(matrix_to_return.cols != -1 && matrix_to_return.rows != -1){
        // allocate matrix
        matrix_to_return.numbers = calloc(matrix_to_return.rows,sizeof(int*));
        for(int i=0; i<matrix_to_return.rows; i++){
            matrix_to_return.numbers[i] = calloc(matrix_to_return.cols,sizeof(int));
        }

        // opening file where we will be reading content
        FILE *matrix = fopen(path_to_matrix,"r");
        size_t length = 0;
        char *line = NULL;
        int row_index = 0;

        // reading lines one by one
        while(getline(&line,&length,matrix) != EOF){
            int col_index = 0;
            line = strtok(line," ");

            // reading columns 
            while(line != NULL){
                matrix_to_return.numbers[row_index][col_index] = atoi(line);
                line = strtok(NULL, " ");
                col_index++;
            }

            row_index++;
        }

        fclose(matrix);
    }

    return matrix_to_return;
}

// this function creates zeros matrix in file
void create_empty_matrix(int rows, int cols, char* filename){
    FILE* file = fopen(filename, "w+");
   
    // printing zeros to file
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            fprintf(file, "%d", 0);
            
            if (j<cols-1) {
                fprintf(file, " ");
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);
}

// it multiplies matrices and returns the result matrix
struct Matrix multiply_matrices(struct Matrix first, struct Matrix second){
    // creating new matrix
    struct Matrix result;
    result.cols = second.cols;
    result.rows = first.rows;

    // allocating memory for matrix
    result.numbers = calloc(result.rows,sizeof(int*));
    for(int i=0; i<result.rows; i++){
        result.numbers[i] = calloc(result.cols,sizeof(int));
    }

    // multiplying all matrices
    for(int i=0; i<first.rows; i++){
        for(int j=0; j<second.cols; j++){
            int cell_value = 0;

            for(int k=0; k<first.cols; k++){
                cell_value += first.numbers[i][k] * second.numbers[k][j];
            }

            result.numbers[i][j] = cell_value;
        }
    }

    return result;
}

// it creates a matrix in file with random values
void create_rand_matrix_in_file(int rows, int columns, char *filename){
    FILE* file = fopen(filename, "w+");
   
    // printinf zeros to file
    for (int i=0; i<rows; i++){
        for (int j=0; j<columns; j++){
            int random_value = rand()%100;
            fprintf(file, "%d",random_value );
            
            if (j<columns-1) {
                fprintf(file, " ");
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);
}