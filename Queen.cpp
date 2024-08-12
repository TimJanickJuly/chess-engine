#include "Queen.h"

#include <iostream>

char Queen::getPieceType() const {
    return 'Q';
}

bool Queen::is_move_possible(const int board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    std::cout << "Current Pos: " << this->get_row_Coord() << ", " << this->get_col_Coord() <<
        "; Target Pos: " << target_coord_row << ", " << target_coord_col <<"\n";
    int row_start = get_row_Coord();
    int col_start = get_col_Coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;
    if(!is_capture && board[target_coord_row][target_coord_col] != 0) {
        return false;
    }


    if (board[target_coord_row][target_coord_col] * piece_color > 0) {
        return false;
    }

    if (row_start == target_coord_row) {
        int min_col = std::min(col_start, target_coord_col);
        int max_col = std::max(col_start, target_coord_col);
        for (int col = min_col + 1; col < max_col; ++col) {
            if (board[row_start][col] != 0) {
                return false;
            }
        }
        return true;
    } else if (col_start == target_coord_col) {
        int min_row = std::min(row_start, target_coord_row);
        int max_row = std::max(row_start, target_coord_row);
        for (int row = min_row + 1; row < max_row; ++row) {
            if (board[row][col_start] != 0) {
                return false;
            }
        }
        return true;

    } else if (abs(row_start - target_coord_row) == abs(col_start - target_coord_col)) {
        int row_step = (target_coord_row > row_start) ? 1 : -1;
        int col_step = (target_coord_col > col_start) ? 1 : -1;
        int row = row_start + row_step;
        int col = col_start + col_step;
        while (row != target_coord_row && col != target_coord_col) {
            if (board[row][col] != 0 && (row != target_coord_row && col != target_coord_col)) {
                return false;
            }
            row += row_step;
            col += col_step;
        }
        return true;
    }
    return false;
}
