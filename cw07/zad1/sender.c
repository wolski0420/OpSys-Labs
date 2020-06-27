#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

#include "types.h"
#include "common.h"

int main(int argc, char **argv){
    // setting up
    key_t main_key = ftok("main",1);
    int semaphores_set = semget(main_key, 4, 0);
    int shared_memory = shmget(main_key, sizeof(memory), 0);
    struct sembuf *start_ops = create_ops(INDEX_PACK, -1);
    struct sembuf *end_ops = create_ops(INDEX_SPACE, 1);

    // working
    while(1){
        // operations
        semop(semaphores_set, start_ops, 2);
        memory *mem = shmat(shared_memory, NULL, 0);    // setting up memory space

        mem->packages[mem->index].n /= 2;
        mem->packages[mem->index].n *=3;
        mem->packages[mem->index].status = STATUS_SENT;
        mem->index = (mem->index+1)%MAX_PACKAGES_COUNT;
        mem->size--;

        // counting actual statistics
        int got_number = semctl(semaphores_set, INDEX_GET, GETVAL);
        int packed_number = semctl(semaphores_set, INDEX_PACK, GETVAL);

        // printing statistics
        printf("(%d %lu) Wyslalem zamowienie o wielkosci %d. Liczba paczek do przygotowania: %d. Liczba paczek do wyslania: %d\n"
                    , getpid(), time(NULL), mem->packages[mem->index].n, got_number, packed_number);

        // changing operations and detaching
        semop(semaphores_set, end_ops, 2);
        shmdt(mem);

        sleep(1);         
    }
}