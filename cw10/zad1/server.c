#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "board.h"
#include "types.h"

// global variables
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
client* clients[MAX_NUMBER_OF_PLAYERS] = {NULL};
int clients_count = 0;
int loc_socket;
int net_socket;

// it is used to exit program when it is not correctly validated
int validation_error(char *message){
    printf("Error! %s\n",message);
    exit(-1);
}

// function to poll local and network sockets
int sockets_polling(int loc_socket, int net_socket){
    // creating sockets
    struct pollfd* pollfds = calloc(clients_count+2, sizeof(struct pollfd));
    pollfds[0].fd = loc_socket;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = net_socket;
    pollfds[1].events = POLLIN;

    // locking mutex
    pthread_mutex_lock(&clients_mutex);

    // rewriting
    for (int i=0; i<clients_count; i++) {
        pollfds[i+2].fd = clients[i]->file_descriptor;
        pollfds[i+2].events = POLLIN;
    }
    
    // unlocking mutex
    pthread_mutex_unlock(&clients_mutex);

    // polling
    poll(pollfds, clients_count+2, -1);

    int index;
    for (index=0; index<clients_count+2; index++)
        if (pollfds[index].revents & POLLIN) break;

    int val = pollfds[index].fd;
    if (val == loc_socket || val == net_socket)
        val = accept(val, NULL, NULL);

    free(pollfds);

    return val;
}

// function to get player index by nickname
int get_by_nick(char* nick){
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->nickname, nick) == 0)
            return i;
        
    return -1;
}

// function to get opponent by index of player
int get_opponent(int index){ 
    if(index%2 == 0) return index+1;
    return index-1;
}

// function to add client and eventually connect him with waiting player
int add_client(char* nick, int fd) {
    if (get_by_nick(nick) != -1) return -1;
    
    // checking for free player to play
    int index = -1;
    for (int i=0; i<MAX_NUMBER_OF_PLAYERS && index==-1; i+=2)
        if (clients[i] != NULL && clients[i + 1] == NULL) 
            index = i + 1;
        
    // if noone is waiting, take first free place
    for (int i=0; i<MAX_NUMBER_OF_PLAYERS && index==-1; i++)
        if (clients[i] == NULL)
            index = i;

    // creating client
    if (index != -1) {
        client* new = calloc(1, sizeof(client));
        new->alive = 1;
        new->file_descriptor = fd;
        new->nickname = calloc(MAX_LENGTH_OF_MESSAGE, sizeof(char));
        strcpy(new->nickname, nick);

        clients[index] = new;
        clients_count++;
    }

    return index;
}

// function to remove client with his opponent
void remove_client(char* nick) {
    printf("Removing client:  : %s\n", nick);

    // getting client
    int index = get_by_nick(nick);
    if (index == -1) return;

    // removing him
    free(clients[index]->nickname);
    free(clients[index]);
    clients[index] = NULL;
    clients_count--;

    // getting his oponent
    int opp_index = get_opponent(index);
    if(opp_index == -1) return;

    // removing opponent
    if (clients[opp_index] != NULL) {
        printf("Removing his opponent\n");

        free(clients[opp_index]->nickname);
        free(clients[opp_index]);
        clients[opp_index] = NULL;
        clients_count--;
    }
}

// function to inform about server waiting and to remove client or send them message
void pinging() {
    printf("=====ServerPinging=====\n");

    // locking mutex
    pthread_mutex_lock(&clients_mutex);

    // removing clients if neccessary
    for (int i=0; i<MAX_NUMBER_OF_PLAYERS; i++) {
        if (clients[i] != NULL && !clients[i]->alive) {
            printf("Removing ping: %s\n", clients[i]->nickname);
            remove_client(clients[i]->nickname);
        }
    }

    // sending ping information to clients
    for (int i=0; i<MAX_NUMBER_OF_PLAYERS; i++) {
        if (clients[i] != NULL) {
            printf("Sending:  :%s\n",clients[i]->nickname);
            send(clients[i]->file_descriptor, "Ping: ", MAX_LENGTH_OF_MESSAGE, 0);
            clients[i]->alive = 0;
        }
    }

    // unlock mutex
    pthread_mutex_unlock(&clients_mutex);

    // repeat (it has to do it all time)
    sleep(2);
    pinging();
}

// function for server work 
void server_working(){
    // it is working all time:
    while (1) {
        // polling sockets and getting descriptor of player
        int client_file_descriptor = sockets_polling(loc_socket, net_socket);

        // getting message from player
        char buffer[MAX_LENGTH_OF_MESSAGE];
        recv(client_file_descriptor, buffer, MAX_LENGTH_OF_MESSAGE, 0);
        printf("%s\n",buffer);

        // spliting message
        char* cmd = strtok(buffer, ":");
        char* arg = strtok(NULL, ":");
        char* nick = strtok(NULL, ":");

        // locking mutex
        pthread_mutex_lock(&clients_mutex);

        // getting decision from player
        if (strcmp(cmd, "Add") == 0) {
            // adding client if it is possible
            int index = add_client(nick, client_file_descriptor);

            // sending information to client about adding process result
            if (index == -1) {      // name taken
                send(client_file_descriptor, "Add:name_occupied", MAX_LENGTH_OF_MESSAGE, 0);
                close(client_file_descriptor);
            } else if (index % 2 == 0) {    // no opponent for him
                send(client_file_descriptor, "Add:no_opponent", MAX_LENGTH_OF_MESSAGE, 0);
            } else {    
                // sending information about X/O 
                int random_number = rand() % 2;
                int player_index = index - random_number;
                int second_player_index = get_opponent(player_index);

                send(clients[player_index]->file_descriptor, "Add:O", MAX_LENGTH_OF_MESSAGE, 0);
                send(clients[second_player_index]->file_descriptor, "Add:X", MAX_LENGTH_OF_MESSAGE, 0);
            }
        }
        if (strcmp(cmd, "Move") == 0) {
            // moving option
            int move = atoi(arg);
            int player = get_by_nick(nick);

            sprintf(buffer, "Move:%d", move);
            send(clients[get_opponent(player)]->file_descriptor, buffer, MAX_LENGTH_OF_MESSAGE, 0);
        }
        if (strcmp(cmd, "Quit") == 0) remove_client(nick);      // player quit if it is called
        if (strcmp(cmd, "--ClientPinging") == 0)       // player pong on server
            if (get_by_nick(nick) != -1) 
                clients[get_by_nick(nick)]->alive = 1;
        
        // unlock mutex
        pthread_mutex_unlock(&clients_mutex);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) return validation_error("Wrong number of arguments!");

    srand(time(NULL));

    // creating local socket
    struct sockaddr_un *sockaddr_un_local = calloc(1,sizeof(struct sockaddr_un));
    memset(sockaddr_un_local, 0, sizeof(struct sockaddr_un));
    sockaddr_un_local->sun_family = AF_UNIX;
    strcpy(sockaddr_un_local->sun_path, argv[2]);
    unlink(argv[2]);

    loc_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(loc_socket, (struct sockaddr*)sockaddr_un_local, sizeof(struct sockaddr_un));
    listen(loc_socket, MAX_BACKLOG);

    // creating network socket
    struct addrinfo *address_info;
    struct addrinfo *hints = calloc(1,sizeof(struct addrinfo));
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_flags = AI_PASSIVE;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_family = AF_UNSPEC;
    
    getaddrinfo(NULL, argv[1], hints, &address_info);
    net_socket = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);

    bind(net_socket, address_info->ai_addr, address_info->ai_addrlen);
    listen(net_socket, MAX_BACKLOG);
    freeaddrinfo(address_info);
    pthread_t *thread = calloc(1,sizeof(pthread_t));
    pthread_create(thread, NULL, (void* (*)(void*))pinging, NULL);

    // server starts working
    server_working();
}