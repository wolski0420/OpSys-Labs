#include "board.h"

// it creates new board
board new_board() {
    // creating empty board
    board game_board;

    // setting up start values
    game_board.move_o = 1;
    for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
            game_board.fields[i][j] = E;

    return game_board;
}

// it makes move on board
int move(board* game_board, int positionX, int positionY) {
    // checking data correction
    if (positionX < 0 || positionX > 3 || positionY < 0 || positionY > 3 || game_board->fields[positionY][positionX] != E) return 0;

    // setting X or O on board
    if(game_board->move_o == 1) game_board->fields[positionY][positionX] = O;
    else game_board->fields[positionY][positionX] = X;

    // changing symbol
    game_board->move_o = 1-game_board->move_o;

    return 1;
}

// it checks rows equality
board_field check_rows(board* game_board) {
    for (int y=0; y<3; y++) {
        // getting all fields from row
        board_field first = game_board->fields[y][0];
        board_field second = game_board->fields[y][1];
        board_field third = game_board->fields[y][2];

        // checking equality
        if (first == second && first == third && first != E) return first;
    }
    return E;
}

// it checks columnsequality
board_field check_columns(board* game_board) {
    for (int x=0; x<3; x++) {
        // getting all fields from column
        board_field first = game_board->fields[0][x];
        board_field second = game_board->fields[1][x];
        board_field third = game_board->fields[2][x];

        // checking equality
        if (first == second && first == third && first != E) return first;
    }
    return E;
}

// it checks diagonals equality
board_field check_diagonals(board* game_board) {
    // getting every field from first diagonal
    board_field first = game_board->fields[0][0];
    board_field second = game_board->fields[1][1];
    board_field third = game_board->fields[2][2];

    // checking
    if (first == second && first == third && first != E) return first;

    // getting every field from second diagonal
    first = game_board->fields[0][2];
    third = game_board->fields[2][0];

    // checking
    if (first == second && first == third && first != E) return first;
    return E;
}

// it checks everything
board_field check_win(board* game_board) {
    // checking all possibilities
    board_field column = check_columns(game_board);
    board_field row = check_rows(game_board);
    board_field diagonal = check_diagonals(game_board);

    // returning
    if(column != E) return column;
    if(row != E) return row;
    return diagonal;
}