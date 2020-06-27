#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "board.h"
#include "types.h"

// mutex
pthread_mutex_t reply_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reply_cond = PTHREAD_COND_INITIALIZER;

// global variables
char buffer[MAX_LENGTH_OF_MESSAGE+1];
char* nick;
char *cmd;
char *arg;

int serv_socket;
int symbol_o;

board game_board;
status_t status = START;

// it is used to exit program when is is not correctly validated
int validation_error(char *message){
    printf("Error! %s\n",message);
    exit(-1);
}

// quit player
void quit() {
    // creating quit message
    char buffer[MAX_LENGTH_OF_MESSAGE + 1];
    sprintf(buffer, "Quit: :%s", nick);

    // sending it
    send(serv_socket, buffer, MAX_LENGTH_OF_MESSAGE, 0);

    // exit program
    exit(0);
}

// checking board if it is won or not
void check_board_status() {
    // check win
    int won = -1;
    board_field field_of_won_game = check_win(&game_board);         // it has to recognise, who won

    // recognizing
    if (field_of_won_game != E) {
        if ((symbol_o && field_of_won_game == O) || (!symbol_o && field_of_won_game == X)){
            printf("You won this game!\n");
            won = 1;
        }
        else{
            printf("You lost this game!\n");
            won = 0;
        } 
    }

    // check draw
    int draw = -1;
    if(won == -1){
        draw = 1;
        for (int i=0; i<3 && draw==1; i++) 
            for(int j=0; j<3 && draw==1; j++)
                if (game_board.fields[i][j] == E) 
                    draw = 0;
    }

    if (draw == 1) printf("Game ended in a draw!\n");       // if draw
    if (won != -1 || draw == 1) status = EXIT;       // if we can't continue
}

// function to draw game board on the console
void draw_board() {
    for (int y=0; y<3; y++) {
        for (int x=0; x<3; x++) {
            if(game_board.fields[y][x] == O) printf("O\t");        // printing circle
            else if(game_board.fields[y][x] == X) printf("X\t"); // printing x
            else printf("%d\t",3*y+x+1);        // printing number of field
        }
        printf("\n");
    }
}

// game function
void game() {
    if (status == START){       // player has to start
        if (strcmp(arg, "name_occupied") == 0){
            printf("This nick is occupied! Choose other!\n");
            exit(1);
        } 
        else if(strcmp(arg, "no_opponent") == 0){          // no opponent for player
            printf("You don't have an opponent! Please wait for others...\n");
            status = WAIT_FOR_ENEMY;
        } 
        else{        // nothing happened, we can start
            // creating game board
            game_board = new_board();

            // setting up symbol for player
            if(arg[0] == 'O') symbol_o = 1;
            else symbol_o = 0;
            
            // moving or waiting for opponent move status
            if(symbol_o == 1) status = MAKE_MY_MOVE;
            else status = WAIT_FOR_MOVE;
        }
    } 
    else if(status == WAIT_FOR_ENEMY){      // waiting for enemy status
        // locking mutex
        pthread_mutex_lock(&reply_mutex);

        // waiting
        while (status != START && status != EXIT) pthread_cond_wait(&reply_cond, &reply_mutex);
        
        // unlocking mutex
        pthread_mutex_unlock(&reply_mutex);

        // creating game board
        game_board = new_board();

        // setting up symbol for player
        if(arg[0] == 'O') symbol_o = 1;
        else symbol_o = 0;

        // moving or waiting for opponent move status
        if(symbol_o == 1) status = MAKE_MY_MOVE;
        else status = WAIT_FOR_MOVE;
    } 
    else if(status == WAIT_FOR_MOVE){       // waiting for opponent move
        printf("Waiting for opponent's move...\n");

        // locking mutex
        pthread_mutex_lock(&reply_mutex);

        // waiting
        while (status != MAKE_ENEMY_MOVE && status != EXIT) pthread_cond_wait(&reply_cond, &reply_mutex);

        // locking mutex
        pthread_mutex_unlock(&reply_mutex);
    } 
    else if(status == MAKE_ENEMY_MOVE){      // getting opponent move
        // processing opponent move
        int move_to_make = atoi(arg);
        move(&game_board, move_to_make%3,move_to_make/3);

        // checking board and eventually changing status
        check_board_status();
        if (status != EXIT) status = MAKE_MY_MOVE;
    } 
    else if(status == MAKE_MY_MOVE){        // moving
        // drawing old board
        draw_board();

        // moving
        int move_to_make;
        do{
            // checking own symbol
            if(symbol_o == 1) printf("Your move (O): ");
            else printf("Your move (X): ");

            // reading move
            scanf("%d", &move_to_make);
            move_to_make--;
        } while (!move(&game_board, move_to_make%3,move_to_make/3));

        // drawing actual board
        draw_board();

        // creating message to send
        char buffer[MAX_LENGTH_OF_MESSAGE + 1];
        sprintf(buffer, "Move:%d:%s", move_to_make, nick);

        // sending message to the server
        send(serv_socket, buffer, MAX_LENGTH_OF_MESSAGE, 0);

        // checking status of the game and eventually changing status for player
        check_board_status();
        if (status != EXIT) status = WAIT_FOR_MOVE;
    } 
    else if(status == EXIT) quit();     // quit game
    
    // loop
    game();
}

// function for client work
void client_working(){
    int game_thread_running = 0;

    while (1) {
        // getting message from server
        recv(serv_socket, buffer, MAX_LENGTH_OF_MESSAGE, 0);
        cmd = strtok(buffer, ":");      // command
        arg = strtok(NULL, ":");        // argument

        // locking mutex
        pthread_mutex_lock(&reply_mutex);

        // checking command
        if (strcmp(cmd, "Add") == 0) {      // add player
            status = START;

            // thread creating (if neccessary)
            if (game_thread_running == 0) {
                pthread_t *thread = calloc(1,sizeof(pthread_t));
                pthread_create(thread, NULL, (void* (*)(void*))game, NULL);
                game_thread_running = 1;
            }
        } 
        else if (strcmp(cmd, "Move") == 0) status = MAKE_ENEMY_MOVE;     // move command
        else if (strcmp(cmd, "Quit") == 0){            // quit command
            status = EXIT;
            exit(0);
        } 
        else if (strcmp(cmd, "Ping") == 0){             // informing server that client is still working
            // creating message
            sprintf(buffer, "--ClientPinging: :%s", nick);

            // and sending it to the server
            send(serv_socket, buffer, MAX_LENGTH_OF_MESSAGE, 0); 
        }

        // unblocking thread and unlocking mutex
        pthread_cond_signal(&reply_cond);
        pthread_mutex_unlock(&reply_mutex);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) return validation_error("Wrong number of arguments!");
    nick = argv[1];

    signal(SIGINT, quit);

    // connection method
    if (strcmp(argv[2], "local") == 0){     // local
        struct sockaddr_un *sockaddr_un_local = calloc(1,sizeof(struct sockaddr_un));
        memset(sockaddr_un_local, 0, sizeof(struct sockaddr_un));
        sockaddr_un_local->sun_family = AF_UNIX;
        strcpy(sockaddr_un_local->sun_path, argv[3]);

        serv_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        connect(serv_socket, (struct sockaddr*)sockaddr_un_local,sizeof(struct sockaddr_un));
    } else {        // network
        struct addrinfo *hints = calloc(1,sizeof(struct addrinfo));
        memset(hints, 0, sizeof(struct addrinfo));
        hints->ai_family = AF_UNSPEC;
        hints->ai_socktype = SOCK_STREAM;

        struct addrinfo *address_info;
        getaddrinfo("localhost", argv[3], hints, &address_info);

        serv_socket = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
        connect(serv_socket, address_info->ai_addr, address_info->ai_addrlen);

        freeaddrinfo(address_info);
    }

    // creating message to send to server
    char buffer[MAX_LENGTH_OF_MESSAGE + 1];
    sprintf(buffer, "Add: :%s", nick);

    // sending message
    send(serv_socket, buffer, MAX_LENGTH_OF_MESSAGE, 0);

    // start client working
    client_working();
}