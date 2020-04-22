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

// global variables (user information and server queue)
int server_queue;
client *this;

// it is used to exit when something's wrong
void error_exit(char *message){
    printf("Error! %s\n",message);
    exit(-1);
}

// function initializing client 
void init_client(){
    char *path_to_home = getpwuid(getuid())->pw_dir;

    this = calloc(1,sizeof(client));

    // creating keys and queues
    key_t server_queue_key = ftok(path_to_home,1);
    if(server_queue_key == -1) error_exit("Cannot create server queue unique key!");

    server_queue = msgget(server_queue_key, 0666);
    if(server_queue == -1) error_exit("Cannot create server queue!");

    key_t client_queue_key = ftok(path_to_home, getpid());
    if(client_queue_key == -1) error_exit("Cannot create client queue unique key!");

    this->queue_id = msgget(client_queue_key, IPC_CREAT | 0666);
    if(this->queue_id == -1) error_exit("Cannot create client queue!");

    // sending message init to server about creation client
    msg init_message;
    init_message.type = INIT;
    sprintf(init_message.text, "%d", this->queue_id);
    msgsnd(server_queue, &init_message, MAX_MSG_LENGTH, 0);

    // receiving information about initialized client
    msgrcv(this->queue_id, &init_message, MAX_MSG_LENGTH, INIT, 0);
    this->id = atoi(init_message.text);
    printf("Your ID in the server is now: %d\n",this->id);
}

// it sends client stop message to server (it finished client work)
void request_stop(){
    // creating and sending message
    msg stop_message;
    stop_message.type = STOP;
    sprintf(stop_message.text, "%d", this->id);
    msgsnd(server_queue, &stop_message, MAX_MSG_LENGTH, 0);

    // deleting user queue and exit
    printf("Deleting client queue... Stopping client...\n");
    msgctl(this->queue_id, IPC_RMID, NULL);
    exit(0);
}

// sending message to other client
void request_send(char *line, int connect_client_queue_id){
    msg send_message;
    send_message.type = SEND;
    sprintf(send_message.text, "%s", strchr(line,' '));
    msgsnd(connect_client_queue_id, &send_message, MAX_MSG_LENGTH, 0);
}

// listing all clients connected with server
void request_list(){
    // creating list message and sending
    msg list_message;
    list_message.type = LIST;
    sprintf(list_message.text, "%d", this->id);
    msgsnd(server_queue, &list_message, MAX_MSG_LENGTH, 0);

    // getting data about clients
    msgrcv(this->queue_id, &list_message, MAX_MSG_LENGTH, LIST, 0);
    printf("%s",list_message.text);
}

// it sends request for disconnect client from chat
void request_disconnect(){
    // creating disconnect message and sending
    msg disconnect_message;
    disconnect_message.type = DISCONNECT;
    sprintf(disconnect_message.text, "%d", this->id);
    msgsnd(server_queue, &disconnect_message, MAX_MSG_LENGTH, 0);

    // only receiving to apply changes
    msgrcv(this->queue_id, &disconnect_message, MAX_MSG_LENGTH, DISCONNECT, 0);
}

// function used to chat between clients
void chat_with_client(int connect_client_queue_id){
    printf("You're chatting with (queue_id): %d\n",connect_client_queue_id);

    // getting client requests
    char line[MAX_MSG_LENGTH];
    while(fgets(line,sizeof(line),stdin) != NULL){
        // firstly checking any waiting messages
        msg message;
        msgrcv(this->queue_id,&message,MAX_MSG_LENGTH, -7L, IPC_NOWAIT);

        // checking
        if(message.type == SEND) printf("%s\n",message.text);  // when someone sent a message
        else if(message.type == DISCONNECT) return;                    // when other client wanted to disconnect
        else{                                                                                                    // when there's no waiting messages
            if(strncmp(line,"SEND",4) == 0) {                                                   // send msg to client in chat
                request_send(line, connect_client_queue_id);
            }
            else if(strncmp(line,"DISCONNECT",10) == 0) {                        // disconnect from chat
                request_disconnect();
                return;
            }
            else if(strncmp(line,"STOP",4) == 0) {                                          // stop client work
                request_disconnect();
                request_stop();
            }
        }

        message.type = -1;
    }
}

// it calls server to connect client with other client
void request_connect(char *args){
    // getting id client who it wants to connect
    strtok(args," ");
    int connect_id = atoi(strtok(NULL," "));

    // creating and sending connect message to server
    msg connect_message;
    connect_message.type = CONNECT;
    sprintf(connect_message.text, "%d %d", this->id, connect_id);
    msgsnd(server_queue, &connect_message, MAX_MSG_LENGTH, 0);

    // getting queue id and starting chat
    msgrcv(this->queue_id, &connect_message, MAX_MSG_LENGTH, CONNECT, 0);
    chat_with_client(atoi(connect_message.text));
}

// it is used to hangle ctrl+c 
void handle_sigint(int sig){
    printf("\n");
    request_stop();
}

int main(int argc, char **argv){
    // initializing all needed queues
    init_client();

    // setting stop signal
    signal(SIGINT, handle_sigint);

    char line[MAX_MSG_LENGTH];
    while(fgets(line,sizeof(line),stdin) != NULL){
        // ready to stop by server (when client want to request server, it checks that server is still working or is stopped)
        msg message;
        msgrcv(this->queue_id,&message,MAX_MSG_LENGTH, -7L, IPC_NOWAIT);

        if(message.type == STOP) request_stop();                                                                        // when server must be stopped
        else if(message.type == CONNECT) chat_with_client(atoi(message.text));           // when other client wanted to connect with current
        else{ 
            // client possible commands to server
            if(strncmp(line,"LIST",4) == 0) request_list();
            else if(strncmp(line,"CONNECT",7) == 0) request_connect(line);       // when connect, he can send, disconnect or stop (check above)
            else if(strncmp(line,"STOP",4) == 0) request_stop();
        }

        message.type = -1;
    }
}