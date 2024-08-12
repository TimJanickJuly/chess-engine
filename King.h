#ifndef KING_H
#define KING_H

#include "Chess_Piece.h"
#include <memory>
#include "game.h"
class King : public Chess_Piece {
public:
    King(int x_coord, int y_coord, const std::string& color)
        : Chess_Piece(x_coord, y_coord, color) {}

    char getPieceType() const override;
    bool is_move_possible(const int board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const override;

private:
    std::weak_ptr<Board> board_ref_;
};

#endif // KING_H
