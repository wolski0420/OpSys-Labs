#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int mode = -2;           // kill = -1, sigqueue = 0, sigrt = 1
int counter = 0;        // counter of catched SIGUSR1

// returns -1 printing error message
int validate_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// it handles continue signal = it counts catched signals send from sender
void continue_signal_handle(int signal_number, siginfo_t *signal_info, void *context){
    counter++;
}

// it finished catching signals and send them back to sender
void finish_signal_handle(int signal_number, siginfo_t *signal_info, void *context){
    // getting given mode and sending signals back in this mode
    if(mode == 0){  // sigqueue
        union sigval queue_value;

        for(int i=0; i<counter; i++){
            queue_value.sival_int = i;
            sigqueue(signal_info->si_pid, SIGUSR1, queue_value);
        }

        sigqueue(signal_info->si_pid, SIGUSR2, queue_value);
    }
    else if(mode == -1){    // kill
        for(int i=0; i<counter; i++){
            kill(signal_info->si_pid, SIGUSR1);
        }

        kill(signal_info->si_pid, SIGUSR2);
    }
    else if(mode == 1){     // sigrt
        for(int i=0; i<counter; i++){
            kill(signal_info->si_pid, SIGRTMIN);
        }

        kill(signal_info->si_pid, SIGRTMAX);
    }
    else printf("[Catcher] Given mode is incorrect!\n");

    // printing number of catched signals in catcher
    printf("[Catcher] Received %d signals\n",counter);

    exit(0);
}


int main(int argc, char **argv){
    // required "./catcher kill|sigqueue|sigrt"
    if(argc <2) return validate_error("Wrong number of arguments!");

    // setting up mask and mode
    sigset_t mask_to_set;
    sigfillset(&mask_to_set);

    if(strcmp(argv[1],"kill") == 0 || strcmp(argv[1],"sigqueue") == 0){
        mode = strcmp(argv[1],"kill") == 0 ? -1:0;
        sigdelset(&mask_to_set,SIGUSR1);
        sigdelset(&mask_to_set,SIGUSR2);
    }
    else if(strcmp(argv[1],"sigrt") == 0){
        mode = 1;
        sigdelset(&mask_to_set,SIGRTMIN);
        sigdelset(&mask_to_set,SIGRTMAX);
    }
    else return validate_error("Wrong first argument!");

    if(sigprocmask(SIG_SETMASK, &mask_to_set, NULL) < 0) return validate_error("Something's wrong during setting mask!");

    // setting up signals: continue is to count catched ones, finish is to let program know that it was last signal
    struct sigaction continue_signal, finish_signal;
    continue_signal.sa_sigaction = continue_signal_handle;
    finish_signal.sa_sigaction = finish_signal_handle;
    continue_signal.sa_flags = SA_SIGINFO;
    finish_signal.sa_flags = SA_SIGINFO;
    sigemptyset(&continue_signal.sa_mask);
    sigemptyset(&finish_signal.sa_mask);

    // setting signal by mode
    if(mode == -1 || mode == 0){
        sigaction(SIGUSR1, &continue_signal, NULL);
        sigaction(SIGUSR2, &finish_signal, NULL);
    }
    else if(mode == 1){
        sigaction(SIGRTMIN, &continue_signal, NULL);
        sigaction(SIGRTMAX, &finish_signal, NULL);
    }

    // printing PID of current process
    printf("[Catcher] Current PID is: %d\n",getpid());

    // waiting for signals
    while(1) sleep(1);

    return 0;
}