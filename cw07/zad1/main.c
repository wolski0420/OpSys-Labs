#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "types.h"
#include "common.h"

// global variables
key_t main_key = -1;
int semaphores_set = -1;
int shared_memory = -1;
pid_t workers_pid[TOTAL_WORKERS];
int workers_counter = 0;

// handler for ctrl+c signal - closing all childs
void handle_sigint(int sig) {
    for (int i=0; i<TOTAL_WORKERS; i++) 
        kill(workers_pid[i], SIGINT);       // sending signal to close process
    
    printf("\nZamykam pracownikow... wylaczam program...");
}

// setting up and control all semaphores
void set_semaphores(){
    semaphores_set = semget(main_key, 4, IPC_CREAT|0666);
    semctl(semaphores_set, INDEX_SPACE, SETVAL, MAX_PACKAGES_COUNT);
    semctl(semaphores_set, INDEX_GET, SETVAL, 0);
    semctl(semaphores_set, INDEX_PACK, SETVAL, 0);
    semctl(semaphores_set, INDEX_MODIFY, SETVAL, 1);
}

// allocating memory and attaching
void set_memory(){
    // allocate
    shared_memory = shmget(main_key, sizeof(memory), IPC_CREAT|0666);

    // attaching
    memory* mem = shmat(shared_memory, NULL, 0);
    for (int i = 0; i<MAX_PACKAGES_COUNT; i++) {
        mem->packages[i].status = STATUS_SENT;
        mem->packages[i].n = 0;
    }

    mem->index = -1;
    mem->size = 0;

    //detaching
    shmdt(mem);
}

int main() {
    // startup
    main_key = ftok("main", 1);
    set_semaphores();
    set_memory();

    // setting ctrl+c signal handler
    signal(SIGINT, handle_sigint);

    // getters working
    for (int i=0; i<GETTERS_NUMBER; i++) 
        if ((workers_pid[workers_counter++]=fork()) == 0) {
            execlp("./getter", "./getter", NULL);
            return 1;
        }

    // packers working
    for (int i=0; i<PACKERS_NUMBER; i++) 
        if ((workers_pid[workers_counter++]=fork()) == 0) {
            execlp("./packer", "./packer", NULL);
            return 1;
        }

    // senders working
    for (int i=0; i<SENDERS_NUMBER; i++)
        if ((workers_pid[workers_counter++]=fork()) == 0) {
            execlp("./sender", "./sender", NULL);
            return 1;
        }

    // waiting for all workers
    for (int i=0; i<TOTAL_WORKERS; i++) wait(0);
 
    if (semaphores_set != -1) semctl(semaphores_set, 0, IPC_RMID);          // removing semaphores set
    if (shared_memory != -1) shmctl(shared_memory, IPC_RMID, NULL);  // removing shared memory

    return 0;
}