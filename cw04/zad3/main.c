#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// it works when we create process
void sigchild_handle(int sig_number, siginfo_t *sig_info, void *context){
    printf("Status value: %d \nReal user ID: %d \nUser time consumed: %ld \n",sig_info->si_status,sig_info->si_uid,sig_info->si_utime);
}

// it works before we finish program
void sigint_handle(int sig_number, siginfo_t *sig_info, void *context){
    printf("Signal code: %d \nTime overrun count: %d \nSystem time consumed: %ld \n",sig_info->si_code,sig_info->si_overrun,sig_info->si_stime);
}

// it works when we want to divide by 0
void sigfpe_handle(int sig_number, siginfo_t *sig_info, void *context){
    printf("Errno value: %d \nFile descriptor: %d \nNumber of attempted system call: %d \n",sig_info->si_errno,sig_info->si_fd,sig_info->si_syscall);
    exit(0);
}

int main(int argc, char **argv){
    // calling: "./output {child|exit|zero}"
    if(argc < 2){
        printf("Wrong number of arguments!\n");
        exit(-1);
    }

    // creates signal
    struct sigaction actionSIG;
    sigemptyset(&actionSIG.sa_mask);
    actionSIG.sa_flags = SA_SIGINFO;

    if(strcmp(argv[1],"child") == 0){                           // setting up child signal and sending it
        actionSIG.sa_sigaction = sigchild_handle;
        sigaction(SIGCHLD,&actionSIG,NULL);
        pid_t child = fork();

        if(child == 0)exit(1);
        
        wait(NULL);
    }
    else if(strcmp(argv[1],"exit") == 0){                   // setting up exit signal and sending it
        actionSIG.sa_sigaction = sigint_handle;
        sigaction(SIGINT, &actionSIG, NULL);
        pause();
    }
    else if(strcmp(argv[1],"zero") == 0){                  // setting up zero signal and sending it
        actionSIG.sa_sigaction = sigfpe_handle;
        sigaction(SIGFPE, &actionSIG, NULL);
        int zero = 0;
        int divider = 1/zero;
    }

    return 0;
}