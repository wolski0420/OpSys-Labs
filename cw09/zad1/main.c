#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "types.h"

// global variables (common for all threads)
int number_of_seats;
int number_of_clients;
int is_barber_sleeping;
int* all_seats;

// mutex and conditions needed
pthread_mutex_t seats_number_changed = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wake_up_barber = PTHREAD_COND_INITIALIZER;

// it exit program when wrong data given in arguments
int validation_error(char *message){
    printf("Error! %s\n",message);
    exit(-1);
}

// method counting number of free seats
int get_free_seats_number() {
    int counter = 0;
    for (int i=0; i<number_of_seats; i++) 
        if (all_seats[i] == 0) counter++;

    return counter;
}

// it is function for client
void client_work(int* id) {
    pthread_mutex_lock(&seats_number_changed);

    // he will be waiting in room if there will be any free seat
    int free_seats_count = get_free_seats_number();
    if (free_seats_count == 0) {
        printf("Zajete; %d\n", *id);
        pthread_mutex_unlock(&seats_number_changed);
        sleep(rand()%MAX_RETRY_CLIENT_TIME+1);
        client_work(id);
        return;
    }

    // if he find a seat, he sit on it
    for (int i=0; i<number_of_seats; i++) {
        if (all_seats[i] == 0) {
            all_seats[i] = *id;
            break;
        }
    }

    // giving changed information about free seats
    printf("Poczekalnia, wolne miejsca: %d; %d\n", free_seats_count-1, *id);

    // waking up barber if it is necessary
    if (free_seats_count == number_of_seats && is_barber_sleeping) {
        printf("Budze golibrode; %d\n", *id);
        pthread_cond_broadcast(&wake_up_barber);
    }
    
    pthread_mutex_unlock(&seats_number_changed);
}

// it is function for barber
void barber_work() {
    int cut_clients_counter = 0;

    // he will be working as long as he won't shave all clients
    while (cut_clients_counter<number_of_clients) {
        pthread_mutex_lock(&seats_number_changed);

        // he is waiting for clients - he likes sleeping :D 
        while (get_free_seats_number() == number_of_seats) {
            printf("Golibroda: ide spac\n");       
            is_barber_sleeping = 1;
            pthread_cond_wait(&wake_up_barber, &seats_number_changed);      // a kind of signal - when someone come to him, he wakes up
        }
        is_barber_sleeping = 0;

        // he is looking for sitting client
        int index;
        for (index=0; index<number_of_seats; index++)
            if (all_seats[index] != 0) break;

        // barber giving info
        int clients_waiting_counter = number_of_seats - get_free_seats_number() - 1;
        printf("Golibroda: czeka %d klientow, gole klienta ID=%d\n",clients_waiting_counter, all_seats[index]);
        
        // changing information about seats - one client sitting less, barber is sending this information
        all_seats[index] = 0;
        cut_clients_counter++;
        pthread_mutex_unlock(&seats_number_changed);

        // shaving
        sleep(rand()%MAX_SHAVE_TIME + 1);
    }

    // work finish
    printf("Golibroda: koniec pracy\n");
}

// function to run all threads
void run_threads(){
    // barber thread
    pthread_t barber_thread;
    pthread_create(&barber_thread, NULL, (void*(*)(void*))barber_work, NULL);

    // clients' threads and id's
    int* clients_id = calloc(number_of_clients, sizeof(int));
    pthread_t* client_threads = calloc(number_of_clients, sizeof(pthread_t));

    for (int i=0; i<number_of_clients; i++) {
        sleep(rand()%MAX_CREATE_CLIENT_TIME+1);
        clients_id[i] = i+1;
        pthread_create(&client_threads[i], NULL, (void*(*)(void*))client_work,clients_id+i);
    }

    // closing
    for (int i=0; i<number_of_clients; i++) pthread_join(client_threads[i], NULL);
    pthread_join(barber_thread, NULL);

    free(clients_id);
    free(client_threads);
}

int main(int argc, char **argv) {
    // validating given args
    if(argc != 3) return validation_error("Wrong number of arguments!");

    number_of_seats = atoi(argv[1]);
    if(number_of_seats <= 0) return validation_error("Wrong given number of seats!");

    number_of_clients = atoi(argv[2]);
    if(number_of_clients <= 0) return validation_error("Wrong given number of clients!");

    // starting shaving by running all threads
    srand(time(NULL));
    all_seats = calloc(number_of_seats, sizeof(int));
    
    run_threads();

    free(all_seats);
}