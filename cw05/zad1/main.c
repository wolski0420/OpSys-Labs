#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// constant variables
const int MAX_NUMBER_OF_COMMANDS = 20;
const int MAX_NUMBER_OF_ARGUMENTS = 10;

// used to return error message during validation
int validation_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// this method counts number of blocks in file
long get_number_of_blocks(FILE *file){
    fseek(file, 0, SEEK_END);
    long number_of_blocks = ftell(file);
    fseek(file, 0, SEEK_SET);                       // setting pointer on the first character
    return number_of_blocks;
}

//  it returns all instructions in one line
char *get_instructions_from_file(FILE *file){
    long size = get_number_of_blocks(file);
    char *commands = calloc(size+1,sizeof(char));

    if(fread(commands,1,size,file) != size) return NULL;
    fseek(file,0,SEEK_SET);                 // maybe not required

    return commands;
}

// it creates and returns empty (NULLED) array to hold single commands with arguments
char ***create_divided_commands_array(){
    char ***to_return = calloc(MAX_NUMBER_OF_COMMANDS,sizeof(char**));
    for(int i=0; i<MAX_NUMBER_OF_COMMANDS; i++){
        to_return[i] = calloc(MAX_NUMBER_OF_ARGUMENTS,sizeof(char*));
        for(int j=0; j<MAX_NUMBER_OF_ARGUMENTS; j++){
            to_return[i][j] = NULL;
        }
    }

    return to_return;
}

// it deletes allocated memory for array of commands
void free_divided_commands_array(char ***divided_commands_array){
    for(int i=0; i<MAX_NUMBER_OF_COMMANDS; i++){
        for(int j=0; j<MAX_NUMBER_OF_ARGUMENTS; j++){
            if(divided_commands_array[i][j] != NULL) free(divided_commands_array[i][j]);
        }
        free(divided_commands_array[i]);
    }
    free(divided_commands_array);
}

// it divides all commands from one line to N lines with names and arguments separated and returns the number of commands
int divide_commands(char ***divided_commands_array, char *connected_commands){
    int commands_counter = 0;
    char *command_tmp = connected_commands;
    char *command = strtok_r(connected_commands,"|",&command_tmp);

    // using strtok_r to not mix all string dividers

    while(command != NULL){         // divides commands
        int index = 0;
        char *argument_tmp = command;
        char *argument = strtok_r(command," ",&argument_tmp);

        while(argument != 0){               // divides arguments
            divided_commands_array[commands_counter][index++] = argument;
            argument = strtok_r(NULL," ",&argument_tmp);
        }

        commands_counter++;
        command = strtok_r(NULL,"|",&command_tmp);
    }

    return commands_counter;
}

// it gets all commands, divide them and execute in given order
void execute(char *connected_commands){
    // creating array to hold instructions
    char ***divided_commands = create_divided_commands_array();
    int number_of_commands = divide_commands(divided_commands,connected_commands);

    // creating pipes
    int all_pipes[MAX_NUMBER_OF_COMMANDS][2];
    for(int i=0; i<number_of_commands-1; i++){
        if(pipe(all_pipes[i]) < 0) exit(1);
    }

    // creating child processes for pipes
    for(int i=0; i<number_of_commands; i++){
        if(fork() == 0){
            // setting input and output for executing command
            if(i != 0) dup2(all_pipes[i-1][0],STDIN_FILENO);
            if(i != number_of_commands-1) dup2(all_pipes[i][1],STDOUT_FILENO);

            for(int j=0; j<number_of_commands-1; j++){
                close(all_pipes[j][0]);
                close(all_pipes[j][1]);
            }

            // executing
            execvp(divided_commands[i][0], divided_commands[i]);
            exit(0);
        }
    }

    for(int i=0; i<number_of_commands-1; i++){
        close(all_pipes[i][0]);
        close(all_pipes[i][1]);
    }

    for(int i=0; i<number_of_commands; i++) wait(0);

    free(divided_commands);    
}

// here is validate
int main(int argc, char **argv){
    // we need "./main file_path"
    if (argc < 2) return validation_error("Wrong number of arguments!");

    // opening file
    FILE *file = fopen(argv[1],"r");
    if(file == NULL) return validation_error("Cannot open the file!");

    // getting all instructions from the file
    char *instructions = get_instructions_from_file(file);
    if(instructions == NULL) return validation_error("Cannot read commands from file!");

    fclose(file);

    // executing every instruction (connected commends)
    char *connected_commands = strtok(instructions,"\n");
    while(connected_commands != NULL){
        execute(connected_commands);
        connected_commands = strtok(NULL,"\n");
    }

    free(connected_commands);
    free(instructions);
}