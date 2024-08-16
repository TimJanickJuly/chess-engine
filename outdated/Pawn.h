#ifndef PAWN_H
#define PAWN_H

#include "Chess_Piece.h"
#include <memory>
#include <tuple>
#include "game.h"
class Board;

class Pawn : public Chess_Piece {
private:
    bool has_moved;
    std::weak_ptr<Board> board_ref;

public:
    Pawn(int row_coord, int col_coord, const std::string& color)
        : Chess_Piece(row_coord, col_coord, color), has_moved(false) {}

    bool is_move_possible(int8_t  board[8][8], int target_coord_row, int target_coord_col, bool is_capture) const override;
    char getPieceType() const override;
    void setMoved();
    bool can_en_passant(int board[8][8], int target_coord_row, int target_coord_col) const;
};

#endif // PAWN_H
