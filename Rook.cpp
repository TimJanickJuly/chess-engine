#include "Rook.h"

#include <iostream>


char Rook::getPieceType() const {
    return 'R';
}
// Prüft, ob der Rook ein Ziel erreichen kann
bool Rook::is_move_possible(const int board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_Coord();
    int col_start = get_col_Coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;
    if (row_start != target_coord_row && col_start != target_coord_col) {
        return false;
    }

    if (row_start == target_coord_row && col_start == target_coord_col) {
        return false;
    }

    if (!is_capture && board[target_coord_row][target_coord_col] * piece_color != 0) {
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
    } else if (col_start == target_coord_col) {
        int min_row = std::min(row_start, target_coord_row);
        int max_row = std::max(row_start, target_coord_row);
        for (int row = min_row + 1; row < max_row; ++row) {
            if (board[row][col_start] != 0) {
                return false;

            }
        }
    }

    return true;
}
