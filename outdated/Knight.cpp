#include "Knight.h"
#include <iostream>

char Knight::getPieceType() const {
    return 'N';  // 'N' steht für Knight (Springer) im Schach
}

bool Knight::is_move_possible(int8_t  board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_Coord();
    int col_start = get_col_Coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    int row_diff = std::abs(target_coord_row - row_start);
    int col_diff = std::abs(target_coord_col - col_start);



    if(!is_capture && board[target_coord_row][target_coord_col] != 0) {
        return false;
    }

    if (board[target_coord_row][target_coord_col] * piece_color > 0) {
        return false;
    }
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        return true;
    }
    return false;
}
