#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int mode = -2;                   // kill = -1, sigqueue = 0, sigrt = 1
int counter = 0;                // counter of catched SIGUSR1
int send_counter = 0;    // number of send signals

// returns -1 printing error message
int validate_error(char *message){
    printf("Error! %s\n",message);
    return -1;
}

// it is used to count catched signals send from catcher
void continue_signal_handle(int signal_number, siginfo_t *signal_info, void *context){
    counter++;

    if(mode == 0 && signal_info != NULL) printf("[SIGQUEUE] Signal number is %d \n",signal_info->si_value.sival_int);
}

// it is used to print number of received and number of send signals
void finish_signal_handle(int signal_number, siginfo_t *signal_info, void *context){
    if(mode == 0 && signal_info != NULL) printf("[SIGQUEUE] Final signal number is %d \n",signal_info->si_value.sival_int);

    printf("[Sender] Sent %d signals\n[Sender] Received %d signals\n",send_counter,counter);

    exit(0);
}


int main(int argc, char **argv){
    // required "./sender PID number kill|sigqueue|sigrt"
    if(argc < 4) return validate_error("Wrong number of arguments!");

    // parsing arguments
    int process_PID = atoi(argv[1]);
    send_counter = atoi(argv[2]);
    if(process_PID < 0 || send_counter < 0) return validate_error("Something's wrong during parsing process PID or number of processes!");

    // setting up mask and mode
    sigset_t mask_to_set;
    sigfillset(&mask_to_set);

    if(strcmp(argv[3],"kill") == 0 || strcmp(argv[3],"sigqueue") == 0){
        mode = strcmp(argv[3],"kill") == 0 ? -1:0;
        sigdelset(&mask_to_set,SIGUSR1);
        sigdelset(&mask_to_set,SIGUSR2);
    }
    else if(strcmp(argv[3],"sigrt") == 0){
        mode = 1;
        sigdelset(&mask_to_set,SIGRTMIN);
        sigdelset(&mask_to_set,SIGRTMAX);
    }
    else return validate_error("Wrong third argument!");

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

    // sending signals to catcher
    if (mode == -1){
        for (int i = 0; i < send_counter; i++){
            kill(process_PID, SIGUSR1);
        }
        
        kill(process_PID, SIGUSR2);
    }
    else if (mode == 0){
        for (int i = 0; i < send_counter; i++){
            sigqueue(process_PID, SIGUSR1, (union sigval){.sival_int = 0});
        }

        sigqueue(process_PID, SIGUSR2, (union sigval){.sival_int = 0});
    }
    else if (mode == 1){
        for (int i = 0; i < send_counter; i++){
            kill(process_PID, SIGRTMIN);
        }
        
        kill(process_PID, SIGRTMAX);
    }
    else return validate_error("Wrong mode!");

    while(1) sleep(1);

    return 0;
}