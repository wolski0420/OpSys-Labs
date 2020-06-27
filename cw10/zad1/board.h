#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include "types.h"

// enum for fields in board
typedef enum { E, O, X } board_field;

// board structure
typedef struct {
    int move_o;
    board_field fields[3][3];
} board;

// it creates new board
board new_board();

// it makes move on board
int move(board* game_board, int positionX, int positionY);

// it checks column equality
board_field check_columns(board* game_board);

// it checks rows equality
board_field check_rows(board* game_board);

// it checks diagonals equality
board_field check_diagonals(board* game_board);

// it checks everything
board_field check_win(board* game_board);

#endif
