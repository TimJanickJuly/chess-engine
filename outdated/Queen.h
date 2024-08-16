#ifndef QUEEN_H
#define QUEEN_H

#include "Chess_Piece.h"

class Queen : public Chess_Piece {
public:
    Queen(int x_coord, int y_coord, const std::string& color)
        : Chess_Piece(x_coord, y_coord, color) {}

    bool is_move_possible(int8_t  board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const override;
    char getPieceType() const override;
};

#endif // QUEEN_H
