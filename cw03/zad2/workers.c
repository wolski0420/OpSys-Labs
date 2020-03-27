#include "workers.h"

// it returns number of digits of number given in argument
int get_digits_number(int number){
    int test_number = number;
    int counter = 0;
    do{
        test_number /= 10;
        counter++;
    } while(test_number > 0);

    return counter;
}

// this function returns structure with first free(nonmultiplied) column and index of pair where the column is
struct Task get_task(int pairs_number) {
    struct Task task_to_return;

    // default -1 because we can possibly not find any nonmultiplied column
    task_to_return.pair_index = -1;         
    task_to_return.column_index = -1;
    
    // iterating through all pairs and searching 
    for(int i=0; i<pairs_number; i++){
        int counter = get_digits_number(i);

        // creating filename of task: ".program/task%%"
        char* filename = calloc(13+counter+1, sizeof(char));
        char *pair_index_char = calloc(counter + 1, sizeof(char));
        sprintf(pair_index_char,"%d",i);
        strcpy(filename,".program/task");
        strcat(filename,pair_index_char);

        // opening file with task
        FILE* tasks_file = fopen(filename, "r+");
        int file_descriptor = fileno(tasks_file);
        flock(file_descriptor, LOCK_EX);

        // getting line with columns 0/1 (nonmultiplied/multiplied)
        size_t length = 0;
        char *line = NULL;
        getline(&line, &length,tasks_file);

        // searching first zeros in chain 0/1
        char* first_zeros_task = strchr(line, '0');
        int first_zeros_task_index = first_zeros_task != NULL ? first_zeros_task - line : -1;

        // if found, let's change line and return info
        if (first_zeros_task_index >= 0) {
            // changing char
            line[first_zeros_task_index] = '1';

            // setting new line
            fseek(tasks_file,0,0);
            fwrite(line,1,strlen(line),tasks_file);
            fflush(tasks_file);

            // setting information
            task_to_return.pair_index = i;
            task_to_return.column_index = first_zeros_task_index;

            flock(file_descriptor,LOCK_UN);
            fclose(tasks_file);

            free(filename);
            free(pair_index_char);

            break;
        }

        flock(file_descriptor, LOCK_UN);
        fclose(tasks_file);
        
        free(filename);
        free(pair_index_char);
    }
    
    // returning task structure with information
    return task_to_return;
}

// it multiplies col_index from second matrix per every first matrix row and write the result to the one specific file
void multiply(char* a_filename, char* b_filename, int col_index, int pair_index) {
    // parsing matrices from files to structures
    struct Matrix first = load_matrix_from_file(a_filename);
    struct Matrix second = load_matrix_from_file(b_filename);

    // getting number of digits = needed to create file name string
    int col_counter = get_digits_number(col_index);
    int pair_counter = get_digits_number(pair_index);

    // creating file name string (path)
    char *filename = calloc(13+pair_counter+1+col_counter+1,sizeof(char));
    char *pair_index_char = calloc(pair_counter+1,sizeof(char));
    char *col_index_char = calloc(col_counter+1,sizeof(char));
    sprintf(pair_index_char,"%d",pair_index);
    sprintf(col_index_char,"%d",col_index);
    strcpy(filename,".program/part");
    strcat(filename,pair_index_char);
    strcat(filename,"_");
    strcat(filename,col_index_char);

     // opening file to writeresult of multiplication
    FILE *part_file = fopen(filename, "w+");

    // multiplication and writing
    for (int i=0; i<first.rows; i++) {
        int result = 0;

        for (int j=0; j<first.cols; j++) {
            result += first.numbers[i][j] * second.numbers[j][col_index];
        }

        if(i == first.rows -1) fprintf(part_file, "%d", result);
        else fprintf(part_file, "%d\n", result);
    }

    fclose(part_file);
    free(filename);
    free(pair_index_char);
    free(col_index_char);
}

// it multiplies col_index from second matrix per every first matrix row and write the result to the result matrix and to one specific file
void multiply_in_one_file(char* a_matrix, char* b_matrix, char *c_matrix, int col_index) {
    // parsing matrices from files to structures
    struct Matrix first = load_matrix_from_file(a_matrix);
    struct Matrix second = load_matrix_from_file(b_matrix);
    struct Matrix result = load_matrix_from_file(c_matrix);

    // multiplication
    for (int i=0; i<first.rows; i++){
        int cell_value = 0;

        for (int j=0; j<first.cols; j++) {
            cell_value += first.numbers[i][j] * second.numbers[j][col_index];
        }

        result.numbers[i][col_index] = cell_value;
    }

    // opening file with result matrix
    FILE *file = fopen(c_matrix, "r+");
    int file_descriptor = fileno(file);
    flock(file_descriptor, LOCK_EX);

    // writing to file
    fseek(file, 0, SEEK_SET);
    for (int i=0; i<result.rows; i++) {
        for (int j=0; j<result.cols; j++) {
            if (j>0) {
                fprintf(file, " ");
            }

            fprintf(file, "%d", result.numbers[i][j]);
        };

        fprintf(file, "\n");
    }

    flock(file_descriptor, LOCK_UN);
    fclose(file);
}

// here process do what he has to
int process_work(char   **a_matrices, char  **b_matrices, char **c_matrices, int max_time, int mode, int pairs_number) {
    int multi_count = 0;
    time_t start = time(NULL);

    // process has limited time
    while((time(NULL) - start) < max_time){
        // we're getting task - process need something to do
        struct Task task = get_task(pairs_number);      

        // if column has index=-1, it means that all columns are multiplied, so we can finish
        if (task.column_index == -1) break;

        // if we chose mode=1, we want to get result of all multiplications made in one pair in one file
        // if we chose mode=2, we want to get all results of multiplications made in one pair in more than one file, then we want to merge them
        if(mode == 1){
            multiply_in_one_file(a_matrices[task.pair_index], b_matrices[task.pair_index], c_matrices[task.pair_index], task.column_index);
            multi_count++;
        }
        else if(mode == 2){
            multiply(a_matrices[task.pair_index], b_matrices[task.pair_index], task.column_index, task.pair_index);
            multi_count++;
        }
    }

    // we returns the number of multiplications which process has done
    exit(multi_count);
}

// this is the majority function which takes path to the list of matrices and creates processes which will be multiplying these matrices
int manage_process(char *list_path, int processes_number, int max_time, int mode){
    FILE *list = fopen(list_path, "r");

    // counting number of pairs matrices
    int pairs_number = 0;
    size_t length;
    char *line;
    while(getline(&line,&length,list) != EOF){
        pairs_number++;
    }

    // returning to the start of file
    fseek(list,0,0);

    // removing last created temporary directory and creating the new one
    system("rm -rf .program");
    system("mkdir -p .program");

    // creating arrays with names of files with matrices
    char **a_matrices = calloc(pairs_number, sizeof(char*));
    char **b_matrices = calloc(pairs_number, sizeof(char*));
    char **c_matrices = calloc(pairs_number, sizeof(char*)); 
    
    // fields to get lines from file and pairs index
    char input_line[PATH_MAX * 3 + 3];
    int pair_index = 0;

    // iterating through all pairs and adding names to arrays, then
    while (fgets(input_line, PATH_MAX*3 + 3, list) != NULL) {
        // getting names of files
        char *tmp = strtok(input_line, " ");
        a_matrices[pair_index] = calloc(strlen(tmp)+1,sizeof(char));
        strcpy(a_matrices[pair_index],tmp);
        tmp = strtok(NULL," ");
        b_matrices[pair_index] = calloc(strlen(tmp)+1, sizeof(char));
        strcpy(b_matrices[pair_index],tmp);
        tmp = strtok(NULL," ");
        c_matrices[pair_index] = calloc(strlen(tmp)-1, sizeof(char));
        strncpy(c_matrices[pair_index],tmp,strlen(tmp)-1);

        // getting sizes of matrixes
        int a_rows = get_row_number(a_matrices[pair_index]);
        int b_cols = get_col_number(b_matrices[pair_index]);

        if(mode == 1) create_empty_matrix(a_rows, b_cols, c_matrices[pair_index]);

        // getting number of digits in number identifying task
        int counter = get_digits_number(pair_index);

        // creating filename for task:       ".program/task%%"
        char *filename = calloc(13+counter+1, sizeof(char));
        char *pair_index_char = calloc(counter +1,sizeof(char));
        sprintf(pair_index_char,"%d",pair_index);
        strcpy(filename,".program/task");
        strcat(filename,pair_index_char);
        
        // creating zeros chain which will be identifying, which column was multiplied
        char *tasks = calloc(b_cols +1,sizeof(char));
        strcpy(tasks,"0");
        for(int i=0; i<b_cols-1; i++) strcat(tasks,"0");

        // writing chain to task file
        FILE* task_file = fopen(filename, "w+");
        fwrite(tasks, 1, b_cols, task_file);

        free(tasks);
        free(filename);
        free(pair_index_char);
        fclose(task_file);

        pair_index++;
    }

    // creating array of processes PID
    pid_t *child_processes = calloc(processes_number, sizeof(pid_t));

    // starting worker processes
    for (int i=0; i<processes_number; i++) {
        pid_t worker_pid = fork();          // creating worker process

        if (worker_pid == 0){
            exit(process_work(a_matrices, b_matrices, c_matrices, max_time, mode, pairs_number));   // child process starts working
        }
        else{
            child_processes[i] = worker_pid;    // parent process writes child process pid to array
        }
    }

    // printing statistics of processes
    for (int i = 0; i < processes_number; i++) {
        int number_of_multiplications;
        waitpid(child_processes[i], &number_of_multiplications, 0);     // we have to wait for process finish
        printf("Process %d executed %d matrices multiplications\n", child_processes[i],WEXITSTATUS(number_of_multiplications));
    }

    // if we chose second mode, we have to connect all results to common files
    if(mode == 2){
        for(int i=0; i<pairs_number; i++){
            // getting number of digits in number identifying pair
            int counter = get_digits_number(i);

            // creating command to paste all results: "paste -d \" \" .program/part%%* > filename"
            char *command = calloc(26+counter+5+strlen(c_matrices[i])+1, sizeof(char));
            char *pair_index_char = calloc(counter +1,sizeof(char));
            sprintf(pair_index_char,"%d",i);
            strcpy(command,"paste -d \" \" .program/part");
            strcat(command,pair_index_char);
            strcat(command,"_* > ");
            strcat(command,c_matrices[i]);

            // pasting
            system(command);

            free(command);
            free(pair_index_char);
        }
    }

    free(child_processes);
    fclose(list);

    return 0;
}