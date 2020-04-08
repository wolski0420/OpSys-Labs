#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function to print error message and return error value
int validate_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

int main(int argc, char **argv){
    if(argc < 2) return validate_error("Wrong number of arguments!");

    // "cat [file_name] | sort"
    char *command = calloc(4+strlen(argv[1])+8,sizeof(char));
    sprintf(command,"cat %s | sort",argv[1]);

    // calling unix command
    FILE *file = popen(command,"w");
    pclose(file);
}