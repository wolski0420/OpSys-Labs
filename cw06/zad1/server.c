#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>

#include "types.h"

// global variables (server queue id and all clients)
key_t server_queue_key;
int server_queue;
client *clients[MAX_CLIENTS_NUMBER] = {NULL}; 

// it is used to exit when something's wrong
void error_exit(char *message){
    printf("Error! %s\n",message);
    exit(-1);
}

// it finds first free id for client
int find_free_client_id(){
    for(int i=0; i<MAX_CLIENTS_NUMBER; i++){
        if(clients[i] == NULL) return i;
    }

    error_exit("Cannot find first free id for client!\n");
    return -1;
}

// it is used to initialize server
void init_server(){
    // getting keys and queues
    server_queue_key = ftok(getpwuid(getuid())->pw_dir, 1);
    if(server_queue_key < 0) error_exit("Cannot create server queue unique key!");

    server_queue = msgget(server_queue_key, IPC_CREAT|0666);
    if(server_queue < 0) error_exit("Cannot create server queue!");

    printf("Server is initialized and ready!\n");
}

// it creates client and send feedback = assigned id
void handle_init(msg *message){
    // creating client
    client *created_client =  calloc(1,sizeof(client));
    created_client->id = find_free_client_id();
    created_client->client_connected_id = -1;
    created_client->queue_id = atoi(message->text);

    // assigning client
    clients[created_client->id] = created_client;

    // sending assigned id
    msg reply_message;
    reply_message.type = INIT;
    sprintf(reply_message.text, "%d", created_client->id);
    msgsnd(created_client->queue_id, &reply_message, MAX_MSG_LENGTH, 0);
    printf("Client %d joined!\n",created_client->id);
}

// it returns list of clients to sender
void handle_list(msg *message){
    // creating reply
    msg reply_message;
    reply_message.type = LIST;

    // writing clients' data to reply
    strcpy(reply_message.text,"");                          // to clear last msg
    for(int i=0; i<MAX_CLIENTS_NUMBER; i++){
        if(clients[i] != NULL){
            // getting id of client
            char buffer[10];
            sprintf(buffer, "ID:%d",clients[i]->id);
            strcat(reply_message.text,buffer);

            // checking if it is sender
            if(atoi(message->text) == clients[i]->id) strcat(reply_message.text, "(*)");

            // checking chat connection
            if(clients[i]->client_connected_id == -1) strcat(reply_message.text, "\t\tActive");
            else strcat(reply_message.text,"\t\tOccupied");

            strcat(reply_message.text, "\n");
        }
    }

    // sending reply message
    msgsnd(clients[atoi(message->text)]->queue_id, &reply_message, MAX_MSG_LENGTH, 0);
    printf("Client %d requested listing!\n",atoi(message->text));
}

// it is used to connect two clients
void handle_connect(msg *message){
    // getting clients
    client *sender_client = clients[atoi(strtok(message->text, " "))];
    client *connect_client = clients[atoi(strtok(NULL, " "))];

    // pairing
    sender_client->client_connected_id = connect_client->id;
    connect_client->client_connected_id = sender_client->id;

    // creating reply to give them know that they are connected
    msg reply_message;
    reply_message.type = CONNECT;

    // sending
    sprintf(reply_message.text, "%d", connect_client->queue_id);
    msgsnd(sender_client->queue_id, &reply_message, MAX_MSG_LENGTH, 0);

    sprintf(reply_message.text, "%d", sender_client->queue_id);
    msgsnd(connect_client->queue_id, &reply_message, MAX_MSG_LENGTH, 0);

    printf("Client %d and client %d started chatting now!\n",sender_client->id, connect_client->id);
}

// it is used to disconnect clients
void handle_disconnect(msg *message){
    // getting clients' data
    client *sender_client = clients[atoi(message->text)];
    client *connect_client = clients[sender_client->client_connected_id];

    // disconnecting them
    sender_client->client_connected_id = -1;
    connect_client->client_connected_id = -1;

    // sending reply confirming disconnection
    msg reply_message;
    reply_message.type = DISCONNECT;
    msgsnd(connect_client->queue_id, &reply_message, MAX_MSG_LENGTH, 0);
    msgsnd(sender_client->queue_id, &reply_message, MAX_MSG_LENGTH, 0);

    printf("Client %d and client %d finished chatting!\n",sender_client->id, connect_client->id);
}

// it is used to stop client
void handle_stop(msg *message){
    client *to_delete_client = clients[atoi(message->text)];
    msgctl(to_delete_client->queue_id,IPC_RMID, NULL);
    clients[atoi(message->text)] = NULL;
    free(to_delete_client);
    
    printf("Stopped client with ID = %d!\n",atoi(message->text));
}

// it finished server working
void handle_sigint(int signum){
    printf("\n");

    // creating message to all clients
    msg stop_message;
    stop_message.type = STOP;

    // sending to all clients
    int send_messages = 0;
    for(int i=0; i<MAX_CLIENTS_NUMBER; i++){
        if(clients[i] != NULL){
            msgsnd(clients[i]->queue_id, &stop_message, MAX_MSG_LENGTH, 0);
            send_messages++;
        }
    }

    // getting replies from all clients and stopping them
    while(send_messages>0){
        msg received_message;
        msgrcv(server_queue, &received_message, MAX_MSG_LENGTH, STOP, 0);
        handle_stop(&received_message);
        send_messages--;
    }

    // closing server
    msgctl(server_queue,IPC_RMID, NULL);
    printf("Closed the server!\n");
    exit(0);
}

int main(int argc, char **argv){
    // initializing all needed queues
    init_server();

    // setting ctrl+c to finish server immidiately
    signal(SIGINT, handle_sigint);

    // getting requests from clients
    while(1){
        // getting message
        msg message;
        msgrcv(server_queue,&message,MAX_MSG_LENGTH, -7L, 0);

        // checking and handling
        if(message.type == INIT) handle_init(&message);
        else if(message.type == LIST) handle_list(&message);
        else if(message.type == CONNECT) handle_connect(&message);
        else if(message.type == DISCONNECT) handle_disconnect(&message);
        else if(message.type == STOP) handle_stop(&message);
    }
}