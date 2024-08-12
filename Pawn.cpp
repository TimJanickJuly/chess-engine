#include "Pawn.h"
#include "game.h"

char Pawn::getPieceType() const {
    return 'P';
}

void Pawn::setMoved() {
    has_moved = true;
}

bool Pawn::can_en_passant(int board[8][8], int target_coord_col, int target_coord_row) const {
    int row_start = get_row_Coord();
    int col_start = get_col_Coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;

    auto board_shared = board_ref.lock();
    if (!board_shared) return false;

    auto [en_passant_row, en_passant_col] = board_shared->get_enpassant_option();

    if ((en_passant_row == target_coord_row && en_passant_col == target_coord_col) &&
        (abs(target_coord_row - row_start) == 1) &&
        (target_coord_col - col_start == piece_color) &&
        (board[col_start][target_coord_row] == -piece_color)) {
        return true;
        }

    return false;
}

bool Pawn::is_move_possible(const int board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const {
    int row_start = get_row_Coord();
    int col_start = get_col_Coord();
    std::string color = getColor();
    int piece_color = (color == "white") ? 1 : -1;
    int direction = (color == "white") ? 1 : -1;

    if (!is_capture) {
        // test: can pawn move one square ahead
        if (target_coord_row == row_start + (direction * 1) && target_coord_col == col_start) {
            if (board[target_coord_row][target_coord_col] == 0) {
                return true;
            }
        }

        // test: can pawn move tow squares ahead
        if (target_coord_row == row_start + (direction * 2) && target_coord_col == col_start) {
            if((color == "white" && row_start == 1) || (color == "black" && row_start == 6)) {
                if (board[target_coord_row][target_coord_col] == 0 && board[target_coord_row + (-1 * direction * 1)][target_coord_col] == 0) {
                    return true;
                }
            }
        }
    }
    if(is_capture) {
        if ((target_coord_row == row_start + (direction * 1) && target_coord_col == col_start + 1)|| (target_coord_row == row_start + (direction * 1) && target_coord_col == col_start -1)) {

            if (board[target_coord_row][target_coord_col] != 0) {
                return true;
            }
        }

    }
    return false;
}
