#include "Chess_Piece.h"

bool Chess_Piece::is_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    switch (type) {
        case 'P':
            return is_pawn_move_possible(board, target_coord_row, target_coord_col, is_capture);
        case 'N':
            return is_knight_move_possible(board, target_coord_row, target_coord_col, is_capture);
        case 'R':
            return is_rook_move_possible(board, target_coord_row, target_coord_col, is_capture);
        case 'Q':
            return is_queen_move_possible(board, target_coord_row, target_coord_col, is_capture);
        case 'K':
            return is_king_move_possible(board, target_coord_row, target_coord_col, is_capture);
        case 'B':
            return is_bishop_move_possible(board, target_coord_row, target_coord_col, is_capture);
        default:
            return false;
    }
}

bool Chess_Piece::is_pawn_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_coord();
    int col_start = get_col_coord();
    std::string color = getColor();
    int direction = (color == "white") ? 1 : -1;

    if (!is_capture) {
        if (target_coord_row == row_start + direction && target_coord_col == col_start) {
            if (board[target_coord_row][target_coord_col] == 0) {
                return true;
            }
        }
        if (target_coord_row == row_start + 2 * direction && target_coord_col == col_start) {
            if ((color == "white" && row_start == 1) || (color == "black" && row_start == 6)) {
                if (board[target_coord_row][target_coord_col] == 0 && board[row_start + direction][col_start] == 0) {
                    return true;
                }
            }
        }
    }
    if (is_capture) {
        if ((target_coord_row == row_start + direction && target_coord_col == col_start + 1) ||
            (target_coord_row == row_start + direction && target_coord_col == col_start - 1)) {
            if (board[target_coord_row][target_coord_col] != 0) {
                return true;
            }
        }
    }
    return false;
}

bool Chess_Piece::is_knight_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_coord();
    int col_start = get_col_coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    int row_diff = std::abs(target_coord_row - row_start);
    int col_diff = std::abs(target_coord_col - col_start);

    if (!is_capture && board[target_coord_row][target_coord_col] != 0) {
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

bool Chess_Piece::is_rook_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_coord();
    int col_start = get_col_coord();
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

bool Chess_Piece::is_queen_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_coord();
    int col_start = get_col_coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    if (!is_capture && board[target_coord_row][target_coord_col] != 0) {
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
    } else if (std::abs(row_start - target_coord_row) == std::abs(col_start - target_coord_col)) {
        int row_step = (target_coord_row > row_start) ? 1 : -1;
        int col_step = (target_coord_col > col_start) ? 1 : -1;
        int row = row_start + row_step;
        int col = col_start + col_step;
        while (row != target_coord_row && col != target_coord_col) {
            if (board[row][col] != 0) {
                return false;
            }
            row += row_step;
            col += col_step;
        }
        return true;
    }
    return false;
}

bool Chess_Piece::is_king_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_coord();
    int col_start = get_col_coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    int row_diff = std::abs(target_coord_row - row_start);
    int col_diff = std::abs(target_coord_col - col_start);

    if (!is_capture && board[target_coord_row][target_coord_col] != 0) {
        return false;
    }

    if (board[target_coord_row][target_coord_col] * piece_color > 0) {
        return false;
    }

    if (row_diff == 0 && col_diff == 0) {
        return false;
    }

    if (row_diff <= 1 && col_diff <= 1) {
        return true;
    }

    return false;
}

bool Chess_Piece::is_bishop_move_possible(int8_t board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_coord();
    int col_start = get_col_coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    int row_diff = std::abs(target_coord_row - row_start);
    int col_diff = std::abs(target_coord_col - col_start);

    if (row_diff != col_diff) {
        return false;
    }

    if (!is_capture && board[target_coord_row][target_coord_col] != 0) {
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
        if (board[row][col] != 0) {
            return false;
        }
        row += row_step;
        col += col_step;
    }

    if (row == target_coord_row && col == target_coord_col) {
        if (board[row][col] * piece_color > 0) {
            return false;
        }
    }

    return true;
}
