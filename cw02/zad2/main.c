#include "find.h"
#include <string.h>
#include <stdio.h>
#include<stdlib.h>

// function to returning errors during failed validation
int main_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

int main(int argc, char **argv){
    printf("Links \tType \tSize \tLast_access_date\t\tLast_modify_date\t\tPath\n");
    // minimum number of arguments is required
    if(argc < 5) return main_error("Invalid number of arguments!");

    // checking -mtime, -atime parameter
    if(strcmp(argv[3],"-mtime") == 0 || strcmp(argv[3],"-atime") == 0){
        char sign;
        int number;
        
        // checking sign existing in -atime/-mtime parameter
        if(argv[4][0] == '-' || argv[4][0] == '+'){
            // if exists, getting this sign
            sign = argv[4][0];

            // getting number after sign
            char *number_in_string  = calloc(strlen(argv[4])-1,sizeof(char));
            strcpy(number_in_string,&argv[4][1]);
            number = atoi(number_in_string);
            if(number == 0) return main_error("Something's wrong with parsing string to int!");
        }
        else{
            // if sign doesn't exist, parsing string to number
            sign = '!';
            number = atoi(argv[4]);
            if(number < 0) return main_error("Something's wrong with parsing string to int !");
        }

        // counting depth: if -maxdepth is called, then counting; if not, then -1 
        int depth = -1;
        if(argc > 5){
            if(argc != 7) return main_error("Invalid number of arguments!");
            if(strcmp(argv[5],"-maxdepth") != 0) return main_error("Invalid name of the second parameter!");

            // getting depth
            depth = atoi(argv[6]);
            if(depth < 0) return main_error("Maxdepth cannot be negative!");
            if(depth == 0 && argv[6][0] != '0') return main_error("Something's wrong with parsing char to int!");
        }

        return find_time(argv[2],argv[3],sign,number,depth);
    }
    else if(strcmp(argv[3],"-maxdepth") == 0){
        // calling only -maxdepth
        int depth = atoi(argv[4]);
        if(depth < 0) return main_error("Maxdepth cannot be negative!");
        if(depth == 0 && argv[4][0] != '0') return main_error("Something's wrong with parsing char to int (argument of -maxdepth)!");

        return max_depth(argv[2], depth);
    }
    else return main_error("Invalid name of \"find\" option ");

    return 0;
}