#include "Bishop.h"

char Bishop::getPieceType() const {
    return 'B';
}

bool Bishop::is_move_possible(const int board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_Coord();
    int col_start = get_col_Coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    int row_diff = std::abs(target_coord_row - row_start);
    int col_diff = std::abs(target_coord_col - col_start);


    if (row_diff != col_diff) {
        return false;
    }

    if(!is_capture && board[target_coord_row][target_coord_col] != 0) {
        return false;
    }

    if (board[target_coord_row][target_coord_col] * piece_color > 0) {
        return false;
    }

    int row_step = (target_coord_row > row_start) ? 1 : -1;
    int col_step = (target_coord_col > col_start) ? 1 : -1;

    int row = row_start + row_step;
    int col = col_start + col_step;
    while (row != target_coord_row && col != target_coord_col) {
        if (board[row][col] != 0 && ((row != target_coord_row) && (col != target_coord_col))) {
            return false;
        }
        row += row_step;
        col += col_step;
    }

    if ((row == target_coord_row) && (col == target_coord_col)) {
        if (board[row][col] * piece_color > 0) {
            return false;
        }
    }

    return true;
}
