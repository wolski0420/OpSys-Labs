#include "workers.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

// function to returning errors during failed validation
int support_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// it returns 1 if result matrix has the same numbers as the multiplicated one below
int check_correctness_of_multiplication(char *first_matrix, char *second_matrix, char *result_matrix){
    // getting all needed matrices
    struct Matrix first = load_matrix_from_file(first_matrix);
    struct Matrix second = load_matrix_from_file(second_matrix);
    struct Matrix real_result = load_matrix_from_file(result_matrix);
    struct Matrix correct_result = multiply_matrices(first,second);         // raw multiplication

    // checking size
    if(real_result.cols != correct_result.cols || real_result.rows != correct_result.rows) return -1;

    // checking values
    for(int i=0; i<correct_result.rows; i++){
        for(int j=0; j<correct_result.cols; j++){
            if(real_result.numbers[i][j] != correct_result.numbers[i][j]) return -1;
        }
    }

    return 1;
}

int main(int argc, char **argv){
    // checking number of arguments
    if(argc < 5) return support_error("Incorrect number of arguments!");

    //      "./output create min max number"                        - for creating
    //      "./output check processes max_time mode"      - for checking

    if(strcmp(argv[1],"create") == 0){
        // getting minimum rand number
        int min = atoi(argv[2]);
        if(min < 0) return support_error("Incorrect min number argument!!");

        // getting maximum rand number
        int max = atoi(argv[3]);
        if(max < 0) return support_error("Incorrect max number argument!");

        // getting number of pairs
        int number = atoi(argv[4]);
        if(number < 0) return support_error("Incorrect number argument!");

        // removing last created files and creating the new directory for them
        system("rm -rf .files");
        system("mkdir -p .files");
        system("rm lista");

        // here we're creating pairs of matrices
        for(int i=0; i<number; i++){
            // random size
            int first_matrix_rows = rand()%(max-min+1)+min;
            int common_rows_cols = rand()%(max-min+1)+min;
            int second_matrix_cols = rand()%(max-min+1)+min;
            
            // getting number of digits in pair index, needed to create path
            int counter = get_digits_number(i);

            // creating paths to matrices: ".files/a%%.txt"
            char *a_matrix = calloc(8+counter+5,sizeof(char));
            char *b_matrix = calloc(8+counter+5,sizeof(char));
            char *c_matrix = calloc(8+counter+5,sizeof(char));
            char *pair_number_index_char = calloc(counter+1,sizeof(char));
            sprintf(pair_number_index_char,"%d",i);
            strcpy(a_matrix,".files/a");
            strcpy(b_matrix,".files/b");
            strcpy(c_matrix,".files/c");
            strcat(a_matrix,pair_number_index_char);
            strcat(b_matrix,pair_number_index_char);
            strcat(c_matrix,pair_number_index_char);
            strcat(a_matrix,".txt");
            strcat(b_matrix,".txt");
            strcat(c_matrix,".txt");

            // creating random matrices to file
            create_rand_matrix_in_file(first_matrix_rows,common_rows_cols,a_matrix);
            create_rand_matrix_in_file(common_rows_cols,second_matrix_cols,b_matrix);

            // writing names to "lista" file
            char *command = calloc(6 + strlen(a_matrix) + 1 + strlen(b_matrix) + 1 + strlen(c_matrix) + 11, sizeof(char));
            strcpy(command,"echo \"");
            strcat(command,a_matrix);
            strcat(command," ");
            strcat(command,b_matrix);
            strcat(command," ");
            strcat(command,c_matrix);
            strcat(command,"\" >> lista");
            system(command);

            free(command);
            free(a_matrix);
            free(b_matrix);
            free(c_matrix);
            free(pair_number_index_char);
        }
    }
    else if(strcmp(argv[1],"check") == 0){
        // getting number of processes
        int number_of_processes = atoi(argv[2]);
        if(number_of_processes<0) return support_error("Invalid number of processes!");

        // getting maximum time
        int max_time = atoi(argv[3]);
        if(max_time <= 0) return support_error("Invalid number of maximum time for process!");
        
        // getting mode
        int mode = atoi(argv[4]);
        if(mode != 1 && mode !=2) return support_error("Invalid mode argument!");

        // multiplying by processes
        manage_process("lista",number_of_processes,max_time,mode);

        // buffor for reading line
        char input_line[PATH_MAX * 3 + 3];
        FILE *list = fopen("lista","r");

        // reading pairs
        while (fgets(input_line, PATH_MAX*3 + 3, list) != NULL) {
            // getting names of files
            char *tmp = strtok(input_line, " ");
            char *first_matrix = calloc(strlen(tmp)+1,sizeof(char));
            strcpy(first_matrix,tmp);
            tmp = strtok(NULL," ");
            char *second_matrix = calloc(strlen(tmp)+1, sizeof(char));
            strcpy(second_matrix,tmp);
            tmp = strtok(NULL," ");
            char *result_matrix = calloc(strlen(tmp)-1, sizeof(char));
            strncpy(result_matrix,tmp,strlen(tmp)-1);

            // checking correctness of multiplication in given pair
            if(check_correctness_of_multiplication(first_matrix,second_matrix,result_matrix) == 1) printf("Good job! Multiplication is correct!\n");
            else printf(" Badjob! Multiplication isn't correct!\n");

            free(first_matrix);
            free(second_matrix);
            free(result_matrix);
        }

        fclose(list);
    }
    else return support_error("Incorrect mode argument!");

    return 0;
}