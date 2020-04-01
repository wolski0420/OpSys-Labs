#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int stop_identifier = 0;    // this variable decide to continue or pause printing

void sigstp_handle(int sig){   // here we only stop printing
    if(stop_identifier == 1){
        stop_identifier = 0;
    }
    else{
        stop_identifier = 1;
        printf("Oczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    }
}

void sigint_handle(int sig){   // it stops program
    printf("Odebrano sygnal SIGINT\n");
    exit(0);
}

int main(int argc, char **argv){
    // stop signal
    struct sigaction actionSIG;
    actionSIG.sa_handler = sigstp_handle;
    actionSIG.sa_flags = 0;
    sigfillset(&actionSIG.sa_mask);
    sigdelset(&actionSIG.sa_mask,SIGINT);   // blocking all signals without stop and exit
    sigdelset(&actionSIG.sa_mask,SIGTSTP);  // because user should only use these signals
    sigaction(SIGTSTP,&actionSIG,NULL);

    // end signal
    signal(SIGINT, sigint_handle);

    // printing
    while(1){
        if(stop_identifier == 0){
            system("ls -al");
            sleep(1);
        }
        else{
            pause();
        }
    }

    return 0;
}