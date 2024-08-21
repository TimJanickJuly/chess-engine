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
#include <cstdint>
#include <list>
#include <utility>
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

class Game : public std::enable_shared_from_this<Game> {
public:

    std::vector<std::string> game_history_str;

    int active_player;

    std::tuple<int, int> white_king_pos;
    std::tuple<int, int> black_king_pos;

    std::vector<std::shared_ptr<Chess_Piece>> white_pieces;
    std::vector<std::shared_ptr<Chess_Piece>> black_pieces;

    std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> active_pieces;
    std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> passive_pieces;

    bool is_active_player_in_check;
    bool is_passive_player_in_check;

    // for castle rights
    bool has_black_king_moved;
    bool has_white_king_moved;
    bool has_white_a_rook_moved;
    bool has_white_h_rook_moved;
    bool has_black_a_rook_moved;
    bool has_black_h_rook_moved;

    std::string game_state;
    int num_moves_played;
    std::shared_ptr<std::tuple<int,int>> masked_coords;
    std::shared_ptr<std::tuple<int,int>> en_passant_coords;
    bool en_passant_option;

    std::string last_move_status;

    bool check_castle(char castle_type);
    std::vector<std::shared_ptr<Chess_Piece>> find_moving_piece_candidates(std::shared_ptr<Move> move);
    std::shared_ptr<Game> get_shared_ptr() { return shared_from_this(); }
    static std::unordered_map<int, std::string> valueToPiece;
    static std::unordered_map<std::string, int> pieceToValue;
    std::shared_ptr<Chess_Piece> create_piece(int val, int row_coord, int col_coord);
    void execute_move(std::shared_ptr<Chess_Piece> piece, std::shared_ptr<Move> move);
    void execute_castle(char castle_type);
    bool is_square_attacked(const std::tuple<int, int> &square,
                            std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> attacking_pieces,
                            const int8_t board_state[8][8], bool is_defense) const;;
    void print_pieces_debug();
    Game();
    void print_board_state() const;
    bool consider_move(std::shared_ptr<Move> move);
    bool is_checkmate(
    );
    int handle_turn(const std::string &str_player_move);
    bool is_own_king_in_check_after_move(
        const std::shared_ptr<Chess_Piece> piece_to_move,
        std::shared_ptr<Move> move,
        const int8_t board_state[8][8]
    );
    bool is_stalemate();
    bool is_attack_diagonal(std::tuple<int, int> defending_king_pos, std::shared_ptr<Chess_Piece> attacking_piece) const;
    bool is_attack_vertical(std::tuple<int, int> defending_king_pos, std::shared_ptr<Chess_Piece> attacking_piece) const;
    bool is_attack_horizontal(std::tuple<int, int> defending_king_pos, std::shared_ptr<Chess_Piece> attacking_piece) const;
    void clean_up_after_turn();
    std::vector<std::tuple<int, int>> compute_block_squares_diag(
        const std::tuple<int, int>& defending_king_pos,
        int attacking_piece_row,
        int attacking_piece_col
    )const;
    std::vector<std::tuple<int, int>> compute_block_squares_vertical(
        const std::tuple<int, int>& defending_king_pos,
        int attacking_piece_row,
        int attacking_piece_col
    ) const;
    std::vector<std::tuple<int, int>> compute_block_squares_horizontal(
        const std::tuple<int, int>& defending_king_pos,
        int attacking_piece_row,
        int attacking_piece_col
    ) const;
    void switchPlayer();
    void print_history();
    bool is_opponents_king_move_legal(
    const std::shared_ptr<Chess_Piece> defending_king_ptr,
    const int8_t board_state[8][8], int new_row_king, int new_col_king);

    bool is_opponents_move_legal(
    const std::shared_ptr<Chess_Piece> piece_to_move_ptr,
    const int8_t board_state[8][8],
    int new_row,
    int new_col


);

    int8_t board_state[8][8] = {
        {WR, WN, WB, WQ, WK, WB, WN, WR},
        {WP, WP, WP, WP, WP, WP, WP, WP},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {BP, BP, BP, BP, BP, BP, BP, BP},
        {BR, BN, BB, BQ, BK, BB, BN, BR}
    };


    std::vector<std::shared_ptr<Move>> get_available_moves(std::shared_ptr<Chess_Piece> piece, int piece_owner);
    std::list<std::vector<std::shared_ptr<Move>>> get_all_available_moves();
    bool has_piece_available_move(std::shared_ptr<Chess_Piece> piece, int piece_owner);
    void promote_pawn(std::shared_ptr<Chess_Piece>piece_ptr, char promotion_type);
};
#endif // BOARD_H
