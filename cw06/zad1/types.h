#ifndef types_h
#define types_h

// constraints
#define MAX_CLIENTS_NUMBER 20
#define MAX_MSG_LENGTH 128
#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define INIT 4
#define CONNECT 5
#define SEND 6

// structure to hold message info
typedef struct{
    long type;
    char text[MAX_MSG_LENGTH];
} msg;

// structure to hold user info
typedef struct{
    int id;
    int client_connected_id;
    int queue_id;
} client;

#endif