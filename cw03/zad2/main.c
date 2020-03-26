#include "workers.h"
#include <stdio.h>
#include <stdlib.h>

// function to returning errors during failed validation
int main_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// validating input
int main(int argc, char **argv){
    if(argc < 5) return main_error("Wrong number of arguments!");

    int processes_counter = atoi(argv[2]);
    if(processes_counter<=0) return main_error("Incorrect number of processes!");

    int max_time = atoi(argv[3]);
    if(max_time <= 0) return main_error("Incorrect number of maximum time!");

    int mode = atoi(argv[4]);
    if(mode != 1 && mode !=2) return main_error("Incorrect mode number!");

    return manage_process(argv[1],processes_counter,max_time,mode);
}