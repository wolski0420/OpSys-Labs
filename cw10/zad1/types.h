#ifndef TYPES_H
#define TYPES_H

// defined maximum values
#define MAX_LENGTH_OF_MESSAGE 256
#define MAX_NUMBER_OF_PLAYERS 12
#define MAX_BACKLOG 10

// client structure for server
typedef struct {
    char* nickname;
    int alive;
    int file_descriptor;
} client;

// structure for status
typedef enum {
    START,
    WAIT_FOR_ENEMY,
    WAIT_FOR_MOVE,
    MAKE_ENEMY_MOVE,
    MAKE_MY_MOVE,
    EXIT
} status_t;

#endif
