#include "list.h"

// function to returning errors during failed validation
int main_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

int main(int argc, char **argv){
    if(argc < 2) return main_error("Wrong number of arguments!");

    return scan_directory(argv[1]);
}