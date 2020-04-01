#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

// it returns -1 with error message
int validation_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// it handles signals
void siguser_handle(int sig_number){
    printf("Received user signal\n");
}


int main(int argc, char **argv){
    // checking number of arguments
    if(argc < 3) return validation_error("Wrong number of arguments! It must be ./output fork|exec ignore|mask|handler|pending");

    // setting given signal
    if(strcmp(argv[2],"ignore") == 0){
        struct sigaction actionSIG;
        actionSIG.sa_flags = 0;
        actionSIG.sa_handler = SIG_IGN;
        sigaction(SIGUSR1, &actionSIG, NULL);
    }
    else if(strcmp(argv[2],"handler") == 0){
        signal(SIGUSR1, siguser_handle);
    }
    else if(strcmp(argv[2],"pending") == 0 || strcmp(argv[2],"mask") == 0){     // not sure about this, model on example from given upel source
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        int error_identifyier = sigprocmask(SIG_BLOCK, &mask, NULL);
        if(error_identifyier < 0) {
            perror("Something's crashed during blocking signal!\n");
        }
    }
    else return validation_error("Wrong second argument!");

    // sending signal to process
    raise(SIGUSR1);

    // checking visibility
    if(strcmp(argv[2],"pending") == 0 || strcmp(argv[2],"mask") == 0){
        sigset_t sigset;
        sigpending(&sigset);
        if(sigismember(&sigset,SIGUSR1) == 1) printf("User signal is visible in process!\n");
        else printf("User signal is not visible in process!\n");
    }

    // fork or exec, printing visibility 
    if(strcmp(argv[1],"fork") == 0){
        if(fork() == 0){
            // if not pending, we can send signal
            if(strcmp(argv[2],"pending") != 0) raise(SIGUSR1);

            // checking visibility of signal
            if(strcmp(argv[2],"pending") == 0 || strcmp(argv[2],"mask") == 0){
                sigset_t sigset;
                sigpending(&sigset);
                if(sigismember(&sigset,SIGUSR1) == 1) printf("User signal is visible in fork child!\n");
                else printf("User signal is not visible in fork child!\n");
            }
        }
    }
    else if(strcmp(argv[1],"exec") == 0) {
        execl("./exec","./exec",argv[2],NULL);
    }
    else return validation_error("Wrong first argument!");

    return 0;
}