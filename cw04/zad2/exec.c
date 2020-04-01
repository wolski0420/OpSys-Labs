#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

// it returns -1 with error message
int validation_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

int main(int argc, char **argv){
    // checking number of arguments
    if(argc < 2) return validation_error("Wrong number of arguments!");

    // if not pending, we can send signal
    if(strcmp(argv[1],"pending") != 0) raise(SIGUSR1);

    // checking visibility of signal
    if(strcmp(argv[1],"pending") == 0 || strcmp(argv[1],"mask") == 0){
        sigset_t sigset;
        sigpending(&sigset);
        if(sigismember(&sigset,SIGUSR1) == 1) printf("User signal is visible in exec child!\n");
        else printf("User signal is not visible in exec child!\n");
    }

    return 0;
}