#ifndef BOARD_H
#define BOARD_H
#include <iostream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <map>
#include <regex>
#include <algorithm>
#include "move.h"
#include "Chess_Piece.h"
using Coordinates = std::tuple<int, int>;

#define EE 0
#define BK -10
#define BQ -9
#define BB -4
#define BN -3
#define BR -5
#define BP -1
#define WK 10
#define WQ 9
#define WB 4
#define WN 3
#define WR 5
#define WP 1

class Board : public std::enable_shared_from_this<Board> {
public:

    std::tuple<int, int> white_king_pos;
    std::tuple<int, int> black_king_pos;

    std::vector<std::shared_ptr<Chess_Piece>> white_pieces;
    std::vector<std::shared_ptr<Chess_Piece>> black_pieces;

    // for castle rights
    bool has_black_king_moved;
    bool has_white_king_moved;
    bool has_white_a_rook_moved;
    bool has_white_h_rook_moved;
    bool has_black_a_rook_moved;
    bool has_black_h_rook_moved;

    bool black_king_in_check;
    bool white_king_in_check;

    // Getter and Setter for black_king_in_check
    bool get_black_king_in_check() const { return black_king_in_check; }
    void set_black_king_in_check(bool in_check) { black_king_in_check = in_check; }

    // Getter and Setter for white_king_in_check
    bool get_white_king_in_check() const { return white_king_in_check; }
    void set_white_king_in_check(bool in_check) { white_king_in_check = in_check; }
    bool check_castle(std::string current_player, std::string castle_type);
    std::vector<std::shared_ptr<Chess_Piece>> find_moving_piece(const Move& move);
    std::shared_ptr<Board> get_shared_ptr() { return shared_from_this(); }
    static std::unordered_map<int, std::string> valueToPiece;
    static std::unordered_map<std::string, int> pieceToValue;
    std::shared_ptr<Chess_Piece> create_piece(int val, int row_coord, int col_coord);
    std::tuple<int, int> en_passant_option;
    void execute_move(Chess_Piece& piece, int target_coord_row, int target_coord_col, bool is_capture);
    void execute_castle(std::string current_player, std::string castle_type);
    bool test_for_check(const std::tuple<int, int>& king_position,
                       const std::vector<std::shared_ptr<Chess_Piece>>& piece_list,
                       const int board[8][8]) const;

    Board();
    void print_board() const;
    bool consider_move(std::string& str_player_move);
    std::tuple<int, int> get_enpassant_option() { return en_passant_option; }
    void set_enpassant_option(int x, int y) {
        en_passant_option = std::make_tuple(x, y);
    }
    bool test_check_simulation(
        const std::shared_ptr<Chess_Piece>& piece,
        const Move& move,
        const int board_state[8][8],
        const std::vector<std::shared_ptr<Chess_Piece>>* pieces_opponent, bool is_capture
    ) const;

    int board_state[8][8] = {
        {WR, WN, WB, WQ, WK, WB, WN, WR},
        {WP, WP, WP, WP, WP, WP, WP, WP},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {BP, BP, BP, BP, BP, BP, BP, BP},
        {BR, BN, BB, BQ, BK, BB, BN, BR}
    };
    std::string player_turn;
};

#endif // BOARD_H
