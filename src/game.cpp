//
// Created by tim.janick.july on 06.08.2024.
//
#include <iomanip>
#include "game.h"
#include <regex>
#include <algorithm>
#include "move.h"
#include <string>
#include <cctype>
#include "Chess_Piece.h"
#include <cassert>
#include <list>
#include "BoardHashMap.h"


std::unordered_map<int, std::string> Game::valueToPiece = {
    {EE, ".."},
    {BK, "BK"},
    {BQ, "BQ"},
    {BB, "BB"},
    {BN, "BN"},
    {BR, "BR"},
    {BP, "BP"},
    {WK, "WK"},
    {WQ, "WQ"},
    {WB, "WB"},
    {WN, "WN"},
    {WR, "WR"},
    {WP, "WP"}
};

std::unordered_map<std::string, int> Game::pieceToValue = {
    {"EMPTY", EE},
    {"BK", BK},
    {"BQ", BQ},
    {"BB", BB},
    {"BN", BN},
    {"BR", BR},
    {"BP", BP},
    {"WK", WK},
    {"WQ", WQ},
    {"WB", WB},
    {"WN", WN},
    {"WR", WR},
    {"WP", WP}
};



int Game::handle_turn(const std::string &str_player_move) {
    std::shared_ptr<Move> move = std::make_shared<Move>(Move::process_move_syntax(str_player_move));
    if (!move->is_legal_move) {
        return 1;
    }
    if (!consider_move(move)){
        return -1;
    }
        if(is_passive_player_in_check) {
            if (active_player > 0) {
                last_move_status = "Black has been put under check\n";
            }else {
                last_move_status = "White has been put under check\n";
            }
            if(is_checkmate()) {
                last_move_status = "Checkmate Detected\n";
                move->is_mate = true;
                if(active_player > 0) {
                    game_state = "white wins";
                }
                else {
                    game_state = "black wins";
                }
                return 0;
            }
            }
        else {
            if(is_stalemate()){
                last_move_status = "Stalemate Detected\n";
                game_state = "stalemate";
                return 0;
            }
            if(!game_history_hash_map.memorize_board_state(board_state)) {
                last_move_status = "Draw by repetition detected\n";
                game_state = "draw";
                return 0;
            }
        }
        clean_up_after_turn();
        switchPlayer();
        num_moves_played += 1;
        game_history_str.push_back(move->get_algebraic_chess_notation());
        return 2;
    }


void Game::switchPlayer() {
    active_player *= -1;
    std::swap(active_pieces, passive_pieces);
}

bool Game::consider_move(std::shared_ptr<Move> move) {
    std::cout << "Your entered move:\n";
    move->printMove();
    std::cout << "\n";
    if (!move->is_legal_move) {
        last_move_status = "Invalid Move Syntax\n";
        return false;
    }

    int promotion_row = (active_player > 0) ? 7 : 0;
    if(move->is_promotion && (move ->row_target != promotion_row || move->getPieceToMove() != 'P')) {
        return false;
    }

    // Handle Castling Moves

    if (move->is_castling_move == "short" || move->is_castling_move == "long") {
        if (move->is_castling_move == "short") {
                if(check_castle('s')) {
                    execute_castle('s');
                    return true;
                }
            }
        }
        if (move->is_castling_move == "long") {
                if(check_castle('l')) {
                    execute_castle('l');
                    return true;
                }
        }
    
    // Handle non Castling Moves



    // 1. Identify the uniquie Piece that is supposed to execute the given move
    std::vector<std::shared_ptr<Chess_Piece>> possible_movers;
    for (const auto& piece : *active_pieces) {
        // Find all pieces that have Correct Piece Type and can move to specified square without putting own king in check
        if (piece->getPieceType() == move->piece_to_move) {
            if (piece->is_move_possible(board_state, move->row_target, move->col_target, move->is_capture, false)) {
                if (!is_own_king_in_check_after_move(piece, move, board_state)) {
                    possible_movers.push_back(piece);
                }
            }
            else {
                if (piece->getPieceType() == 'P' && move->row_target == std::get<0>(*en_passant_coords) && move->col_target == std::get<1>(*en_passant_coords)) {
                    if(piece->is_en_passant_possible(board_state, move->row_target, move->col_target, move->is_capture, false)) {
                        if (!is_own_king_in_check_after_move(piece, move, board_state)) {
                            possible_movers.push_back(piece);
}
                    }
                }
            }
        }
    }


    if (possible_movers.empty()) {
        return false;
    }
    bool row_unique = true;
    bool col_unique = true;
    // if more than 1 piece has been found, then try to filter them by using specidied start coords in move
    if (possible_movers.size() > 1) {

        move->is_difficult == true;              // cleaning move to display it precisely in Game history
        if(possible_movers.at(0)->get_row() != possible_movers.at(1)->get_row()) {
            row_unique = false;
        }
        if(possible_movers.at(0)->get_col() != possible_movers.at(1)->get_col()) {
            col_unique = false;
        }

        for (auto it = possible_movers.begin(); it != possible_movers.end(); ) {
            const auto& piece = *it;
            if ((move->get_row_CoordStart() != -1 && piece->get_row() != move->get_row_CoordStart()) ||
                (move->get_col_CoordStart() != -1 && piece->get_col()!= move->get_col_CoordStart())) {
                it = possible_movers.erase(it);
                } else {
                    ++it;
                }
        }
    }
    else if(possible_movers.size() == 1){
        move->is_difficult == false;
    }
    // reject the move if its not clear which piece is supposed to move
    if (possible_movers.size() != 1) {
        return false;
    }
    
    std::shared_ptr<Chess_Piece> piece_to_move_ptr = possible_movers.at(0);

    // for clean move syntax in gamehistory
    if(move->is_difficult) {
        if(!row_unique) {

            move->row_start = piece_to_move_ptr->get_row();
        }
        if(!col_unique) {
            move->col_start = piece_to_move_ptr->get_col();
        }
    }

    if(move->getPieceToMove() == 'P' && abs(piece_to_move_ptr->get_row() - move->row_target) == 2) {
        en_passant_option = true;
        int en_passant_row = (active_player > 0) ? 2 : 5;
        en_passant_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(en_passant_row, move->col_target));
    }

    execute_move(piece_to_move_ptr, move);                                                      /// <- Move is executed here


    if(move->is_promotion) {
        promote_pawn(piece_to_move_ptr, move->promotion_type);
    }


    auto passive_king_pos = (active_player > 0) ? black_king_pos : white_king_pos;

    is_active_player_in_check = false;
    is_passive_player_in_check = is_square_attacked(passive_king_pos, active_pieces, board_state, false);
    if(is_passive_player_in_check) {
        move->is_check = true;
    }
    return true;
}

bool Game::is_own_king_in_check_after_move(
    const std::shared_ptr<Chess_Piece> piece_to_move,
    std::shared_ptr<Move> move,
    const int8_t board_state[8][8]
) {
    std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> attacking_pieces;

    attacking_pieces = (active_player > 0) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces) : std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);
    auto defending_king_pos = (active_player > 0)? white_king_pos : black_king_pos;

    // if the piece that is supposed to move is the king itself: test if its new square is safe
    // the coords of his new position are masked s.t. there are no interactions with attacking pieces on
    // the kings new square, which would not exist anymore after his move there.

        if (move->getPieceToMove() == 'K') {
            masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(move->row_target, move->col_target));
            bool result = is_square_attacked(std::make_tuple(move->row_target, move->get_col_CoordTarget()), attacking_pieces, board_state, true);
            masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(-1, -1));
            return result;
        }

    // if the piece to move is not a king then simulate the new board state and check wheter king is safe in this position
    // the coords which are moved to are masked again s.t. a captured piece in the new position cannot attack the king

        int8_t board_state_copy[8][8];
        memcpy(board_state_copy, board_state, sizeof(board_state_copy));

        int target_row = move->get_row_CoordTarget();
        int target_col = move->get_col_CoordTarget();

        int start_row = piece_to_move->get_row();
        int start_col = piece_to_move->get_col();
        int8_t piece_value;

        switch (abs(piece_to_move->getPieceType())) {
            case 'P': piece_value = 1; break;
            case 'N': piece_value = 3; break;
            case 'B': piece_value = 4; break;
            case 'R': piece_value = 5; break;
            case 'Q': piece_value = 9; break;
            case 'K': piece_value = 10; break;
            default: throw std::invalid_argument("Invalid Piece Type");
        }
        if (piece_to_move->getColor() == "black"){piece_value *= -1;}

        board_state_copy[start_row][start_col] = 0;
        board_state_copy[target_row][target_col] = piece_value;
        masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(target_row, target_col));
        bool result = is_square_attacked(defending_king_pos, attacking_pieces, board_state_copy, false);
        masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(-1,-1));
        return result;
    }


bool Game::is_opponents_king_move_legal(
    const std::shared_ptr<Chess_Piece> defending_king_ptr,
    const int8_t board_state[8][8], int new_row_king, int new_col_king
) {

    auto attacking_pieces = active_pieces;
    auto defending_king_pos = (active_player > 0)? black_king_pos : white_king_pos;

    assert(defending_king_ptr->get_row() == std::get<0>(defending_king_pos) && defending_king_ptr->get_col() == std::get<1>(defending_king_pos) && "Error in is_opponents_king_move_legal");

    masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(new_row_king, new_col_king));
    bool result = !is_square_attacked(std::make_tuple(new_row_king, new_col_king), attacking_pieces, board_state, true);
    masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(-1, -1));

    return result;
    }


bool Game::is_opponents_move_legal(
    const std::shared_ptr<Chess_Piece> piece_to_move_ptr,
    const int8_t board_state[8][8],
    int new_row,
    int new_col
) {

    auto attacking_pieces = active_pieces;
    auto defending_king_pos = (active_player > 0)? black_king_pos : white_king_pos;


    int8_t board_state_copy[8][8];
    memcpy(board_state_copy, board_state, sizeof(board_state_copy));

    int start_row = piece_to_move_ptr->get_row();
    int start_col = piece_to_move_ptr->get_col();

    int8_t piece_value;
    switch (abs(piece_to_move_ptr->getPieceType())) {
        case 'P': piece_value = 1; break;
        case 'N': piece_value = 3; break;
        case 'B': piece_value = 4; break;
        case 'R': piece_value = 5; break;
        case 'Q': piece_value = 9; break;
        case 'K': piece_value = 10; break;
        default: throw std::invalid_argument("Invalid Piece Type");
    }
    if (piece_to_move_ptr->getColor() == "black"){piece_value *= -1;}

    board_state_copy[start_row][start_col] = 0;
    board_state_copy[new_row][new_col] = piece_value;

    masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(new_row, new_col));
    bool result = !is_square_attacked(std::make_tuple(std::get<0>(defending_king_pos), std::get<1>(defending_king_pos)), attacking_pieces, board_state_copy, true);
    masked_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(-1, -1));

    return result;
}


bool Game::is_square_attacked(const std::tuple<int, int> &square,
                              std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> attacking_pieces,
                              const int8_t board_state[8][8], bool is_defense) const{

    int row = std::get<0>(square);
    int col = std::get<1>(square);
    for (const auto &piece: *attacking_pieces) {
        if (!(piece->get_row() == std::get<0>(*masked_coords) && piece->get_col() == std::get<1>(*masked_coords))) {
            if (piece->is_move_possible(board_state, row, col, true, is_defense)) {
                return true;
            }
        }
    }
    return false;
}

void Game::execute_move(std::shared_ptr<Chess_Piece> piece, std::shared_ptr<Move> move) {

    int8_t piece_value;

    switch (piece->getPieceType()) {
        case 'P': piece_value = 1; break; // Pawn
        case 'N': piece_value = 3; break; // Knight
        case 'B': piece_value = 4; break; // Bishop
        case 'R': piece_value = 5; break; // Rook
        case 'Q': piece_value = 9; break; // Queen
        case 'K': piece_value = 10; break; // King
        default: throw std::invalid_argument("Unbekannter Figurtyp");
    }

    if (piece->getPieceType() == 'R') {
        int rook_row = (active_player > 0) ? 0 : 7;
        if(!has_black_a_rook_moved && piece->get_row() == rook_row && piece->get_col() == 0) {
            has_black_a_rook_moved = true;
        }
        if(!has_black_h_rook_moved && piece->get_row() == rook_row && piece->get_col() == 7) {
            has_black_h_rook_moved = true;
        }

    }
    if (piece->getPieceType() == 'K') {
        if (active_player > 0) {
            has_white_king_moved = true;
        }else {
            has_black_king_moved = true;
        }
    }

    if (piece->getColor() == "black") {
        piece_value *= -1; // Negative Werte für schwarze Figuren
    }

    bool en_passant = false;
    //test for en passant
    if(piece->getPieceType() == 'P' && board_state[move->row_target][move->col_target] == 0 && piece->get_col() != move->col_target) {
        en_passant = true;
    }


    // Setze die Zielposition auf die Figur
    board_state[move->get_row_CoordTarget()][move->get_col_CoordTarget()] = piece_value;

    // Leere die Startposition
    board_state[piece->get_row()][piece->get_col()] = 0;

    // Aktualisiere die Position der Figur
    piece->set_row(move->get_row_CoordTarget());
    piece->set_col(move->get_col_CoordTarget());

    // Aktualisiere die Position der Könige, falls es sich um einen König handelt
    if (piece->getPieceType() == 'K') {
        if (piece->getColor() == "white") {
            white_king_pos = std::make_tuple(move->get_row_CoordTarget(), move->get_col_CoordTarget());
        } else {
            black_king_pos = std::make_tuple(move->get_row_CoordTarget(), move->get_col_CoordTarget());
        }

    }

    // If a piece has to be captured, then remove it for the piece list
    if (board_state[move->row_target][move->col_target] != 0) {
        //game_history_hash_map.clear_hashmap_history();  // hashmap can be cleared because board_state repetion is impossible after a piece has been captured

        std::vector<std::shared_ptr<Chess_Piece>>* opponent_pieces = (active_player > 0) ? &black_pieces : &white_pieces;

        auto it = std::find_if(opponent_pieces->begin(), opponent_pieces->end(), [&](const std::shared_ptr<Chess_Piece>& p) {
            return p->get_row() == move->get_row_CoordTarget() && p->get_col()== move->get_col_CoordTarget();
        });

        if (it != opponent_pieces->end()) {
            opponent_pieces->erase(it);
        }
    }
    if (en_passant) {
        int direction = (active_player > 0)? 1 : -1;
        board_state[piece->get_row() - direction][piece->get_col()] = 0;
        std::vector<std::shared_ptr<Chess_Piece>>* opponent_pieces = (active_player > 0) ? &black_pieces : &white_pieces;

        auto it = std::find_if(opponent_pieces->begin(), opponent_pieces->end(), [&](const std::shared_ptr<Chess_Piece>& p) {
            return p->get_row() == move->get_row_CoordTarget() - direction && p->get_col()== move->get_col_CoordTarget();
        });

        if (it != opponent_pieces->end()) {
            opponent_pieces->erase(it);
        }
    }
}



void Game::clean_up_after_turn() {
    masked_coords = std::make_shared<std::tuple<int,int>>(-1,-1);

    if(!en_passant_option) {
        en_passant_coords= std::make_shared<std::tuple<int,int>>(-1,-1);
        en_passant_option = false;
    }

    if (board_state[std::get<0>(white_king_pos)][std::get<1>(white_king_pos)] != 10) {
        for (const auto& piece : white_pieces) {
            if(piece->getPieceType() == 'K') {
                white_king_pos = std::make_tuple(piece->get_row(), piece->get_col());
            }
        }
    }
    if (board_state[std::get<0>(black_king_pos)][std::get<1>(black_king_pos)] != -10) {
        for (const auto& piece : black_pieces) {
            if(piece->getPieceType() == 'K') {
                white_king_pos = std::make_tuple(piece->get_row(), piece->get_col());
            }
        }
    }

    active_pieces = (active_player > 0) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces) :std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces);
    passive_pieces = (active_player > 0) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces) :std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);
}

Game::Game() {
    last_move_status  = "---";

    game_state = "running";
    num_moves_played = 0;
    active_player = 1;

    white_king_pos = std::make_tuple(0, 4);
    black_king_pos = std::make_tuple(7, 4);
    
    is_active_player_in_check = false;
    is_passive_player_in_check = false;

    masked_coords = std::make_shared<std::tuple<int,int>>(-1,-1);
    en_passant_coords= std::make_shared<std::tuple<int,int>>(-1,-1);
    en_passant_option = false;

    has_black_king_moved = false;
    has_white_king_moved = false;
    has_white_a_rook_moved = false;
    has_white_h_rook_moved = false;
    has_black_a_rook_moved = false;
    has_black_h_rook_moved = false;

    for (int i = 0; i <= 7; i++) {
        for (int j = 0; j <= 7; j++) {
            if (board_state[i][j] != EE) { // Annahme: EE steht für ein leeres Feld
                auto piece = create_piece(board_state[i][j], i, j);

                if (board_state[i][j] > 0) {
                    white_pieces.push_back(piece);
                } else {
                    black_pieces.push_back(piece);
                }
            }
        }
    }

    active_pieces = std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);
    passive_pieces = std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces);


    // Debug-Ausgabe für weiße Figuren
    std::cout << "White Pieces:\n";
    for (const auto& piece : *active_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row() << ", " << piece->get_col()<< ")\n";
    }

    // Debug-Ausgabe für schwarze Figuren
    std::cout << "Black Pieces:\n";
    for (const auto& piece : *passive_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row() << ", " << piece->get_col()<< ")\n";
    }
}

bool Game::check_castle(char castle_type) {
    auto active_king_pos = (active_player > 0) ? white_king_pos : black_king_pos;
    bool active_player_has_king_moved = (active_player > 0) ? has_white_king_moved : has_black_king_moved;
    bool active_player_a_rook_moved = (active_player > 0) ? has_white_a_rook_moved : has_black_a_rook_moved;
    bool active_player_h_rook_moved = (active_player > 0) ? has_white_h_rook_moved : has_black_h_rook_moved;
    if (castle_type == 's') { // Kurze Rochade
        if (active_player_has_king_moved) {
            last_move_status = "Error: King has been moved\n";
            return false;
        }
        if (active_player_h_rook_moved) {
            last_move_status = "Error: h rook has been moved\n";
            return false;
        }
    }
    else if (castle_type == 'l') { // Lange Rochade
        if (active_player_has_king_moved) {
            last_move_status = "Error: King has been moved\n";
            return false;
        }
        if (active_player_a_rook_moved) {
            last_move_status = "Error: a rook has been moved\n";
            return false;
        }
    }


    int rook_row = (active_player > 0) ? 0 : 7;
    int rook_col = (castle_type == 's') ? 7 : 0;

    if (abs(board_state[rook_row][rook_col]) != 5) {
        last_move_status = "rook is missing\n";
        return false;
    }
    if (abs(board_state[std::get<0>(active_king_pos)][std::get<1>(active_king_pos)]) != 10) {
        last_move_status = "king is missing\n";
        return false;
    }
    if (castle_type == 's') {

        for(int i = 0; i < 3; i++) {
            auto square_to_check = std::make_tuple(std::get<0>(active_king_pos), std::get<1>(active_king_pos) + i);
            if(is_square_attacked(square_to_check, passive_pieces, board_state, false)) {
                last_move_status = "king cant castle over attacked squares\n";
                return false;
            }
            if(i > 0 && board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) { // check if squares are ampty but leave out kings and rooks square
                last_move_status = "castle path is blocked\n";
                return false;
            }
        }


    }
    if (castle_type == 'l') {
        for(int i = 0; i < 4; i++) {
            auto square_to_check = std::make_tuple(std::get<0>(active_king_pos), std::get<1>(active_king_pos) - i);
            if(is_square_attacked(square_to_check, passive_pieces, board_state, false)) {

                return false;
            }
            if(i > 0 && board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) { // check if squares are ampty but leave out kings and rooks square
                return false;
            }
        }
    }
    return true;
}

void Game::execute_castle(char castle_type) {
    int rook_row = (active_player > 0) ? 0 : 7;
    int rook_col = (castle_type == 's') ? 7 : 0;
    std::shared_ptr<Chess_Piece> king;
    std::shared_ptr<Chess_Piece> rook;
    auto active_king_pos = (active_player > 0) ? white_king_pos : black_king_pos;
    // Finde den Turm
    auto it_rook = std::find_if(active_pieces->begin(), active_pieces->end(),
                                [=](const std::shared_ptr<Chess_Piece>& piece) {
                                    return piece->getPieceType() == 'R' &&
                                           piece->get_row() == rook_row &&
                                           piece->get_col()== rook_col;
                                });

    if (it_rook != active_pieces->end()) {
        rook = *it_rook;
    } else {
        std::cerr << "Rook not found for castling!" << std::endl;
        return; // Abbrechen, wenn der Turm nicht gefunden wird
    }

    // Finde den König
    auto it_king = std::find_if(active_pieces->begin(), active_pieces->end(),
                                [=](const std::shared_ptr<Chess_Piece> piece) {
                                    return piece->getPieceType() == 'K' &&
                                           piece->get_row() == std::get<0>(active_king_pos) &&
                                           piece->get_col()== std::get<1>(active_king_pos);
                                });

    if (it_king != active_pieces->end()) {
        king = *it_king;
    } else {
        std::cerr << "King not found for castling!" << std::endl;
        return; // Abbrechen, wenn der König nicht gefunden wird
    }

    if (active_player > 0) {
        has_white_king_moved = true;
        if(castle_type == 's') {
            has_white_h_rook_moved = true;
        }
        if(castle_type == 'l') {
            has_white_a_rook_moved = true;
        }
    }
    else {
        has_black_king_moved = true;
        if(castle_type == 's') {
            has_black_h_rook_moved = true;
        }
        if(castle_type == 'l') {
            has_black_a_rook_moved = true;
        }
    }

        // SWAP KING AND ROOK ON BOARD
    if (castle_type == 's') {
        king->set_col(6);
        rook->set_col(5);
        board_state[rook_row][4] = 0; // Ursprüngliche Position des Königs leeren
        board_state[rook_row][7] = 0; // Ursprüngliche Position des Turms leeren
        board_state[rook_row][5] = (active_player > 0) ? 5 : -5;
        board_state[rook_row][6] = (active_player > 0) ? 10 : -10;

        if(active_player > 0) {
            white_king_pos = std::make_tuple(0,6);
        }else {
            black_king_pos = std::make_tuple(7,6);
        }

    } else if (castle_type == 'l') {
        king->set_col(2);
        rook->set_col(3);
        board_state[rook_row][4] = 0;
        board_state[rook_row][0] = 0;
        board_state[rook_row][3] = (active_player > 0) ? 5 : -5;
        board_state[rook_row][2] = (active_player > 0) ? 10 : -10;
        if(active_player > 0) {
            white_king_pos = std::make_tuple(0,2);
        }else {
            black_king_pos = std::make_tuple(7,2);
        }
    }
}


bool Game::is_checkmate(
){

    auto defending_king_pos = (active_player > 0) ? black_king_pos : white_king_pos;
    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);



    auto attacking_pieces = (active_player > 0 ) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces) : std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces);
    auto defending_pieces = (active_player > 0 ) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces) : std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);


    // Find all Pieces that are checking the king: min 1, max 2
    std::vector<std::shared_ptr<Chess_Piece>> attacking_pieces_that_give_check;
    for (const auto& piece : *attacking_pieces) {
        if (piece->is_move_possible(board_state, king_row, king_col,true, false)) {
        attacking_pieces_that_give_check.push_back(piece);
        }
    }

    //find the pointer to the king object
    std::shared_ptr<Chess_Piece> defending_king_ptr;
    for (const auto& piece : *defending_pieces) {
        if (piece->get_row() == king_row && piece->get_col() == king_col) {
            defending_king_ptr = piece;
        }
    }
    assert(defending_king_ptr != nullptr && "Error in Detcting Checkmate, could not find defending_king_ptr: defending_king_ptr is a nullptr!");

    bool multicheck = (attacking_pieces_that_give_check.size() > 1);

    // Try to move the king to its 8- Neighbourhood
    for (int row = -1; row <= 1; ++row) {
        for (int col = -1; col <= 1; ++col) {
            if (row != 0 || col != 0) {
                int new_row = king_row + row;
                int new_col = king_col + col;
                if (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {   // board constrains
                        if (defending_king_ptr->is_move_possible(board_state, new_row, new_col,true, false)) {
                            if(is_opponents_king_move_legal(defending_king_ptr, board_state, new_row, new_col)) {
                                return false; // King can run to safe square
                                }
                        }
                    }
            }
        }
    }

    if (multicheck) {
        return true; // checkmate, you cannot capture and block more than one attacking piece
    }

    // Try to capture attacker
    std::shared_ptr<Chess_Piece> piece_that_gives_check = attacking_pieces_that_give_check[0];

    for (const auto& piece : *defending_pieces) {
        if (piece->getPieceType() != 'K' && piece->is_move_possible(board_state, piece_that_gives_check->get_row(), piece_that_gives_check->get_col(), true, false)) {
            if(is_opponents_move_legal(piece, board_state, piece_that_gives_check->get_row(), piece_that_gives_check->get_col())) {
                return false; // A defending piece can capture the king attacking piece
                }
        }
    }
    // Try to Body-Block Check
    if (piece_that_gives_check->getPieceType() == 'N' || piece_that_gives_check->getPieceType() == 'P') {
        return true; // Checkmate, you cannot block checks by Knight or Pawn
    }

    int row_diff = std::abs(std::get<0>(defending_king_pos) - piece_that_gives_check->get_row());
    int col_diff = std::abs(std::get<1>(defending_king_pos) - piece_that_gives_check->get_col());


    if (row_diff <= 1 && col_diff <= 1) {
        return true; // Checkmate, you cannot block an attacker that close to the king
    }

    std::vector<std::tuple<int, int>> possible_squares_for_blockers;

    if (row_diff == col_diff) {
        possible_squares_for_blockers= compute_block_squares_diag(defending_king_pos, piece_that_gives_check->get_row(), piece_that_gives_check->get_col());
    } else if (col_diff == 0) {
        possible_squares_for_blockers = compute_block_squares_vertical(defending_king_pos, piece_that_gives_check->get_row(), piece_that_gives_check->get_col());
    } else if (row_diff == 0) {
        possible_squares_for_blockers = compute_block_squares_horizontal(defending_king_pos, piece_that_gives_check->get_row(), piece_that_gives_check->get_col());
    }

    for (const auto& square : possible_squares_for_blockers) {
        int row, col;
        std::tie(row, col) = square;
    }

    for (const auto& square : possible_squares_for_blockers) {
        for (const auto& piece : *defending_pieces) {
            if (piece->getPieceType() != 'K' && piece->is_move_possible(board_state, std::get<0>(square), std::get<1>(square),false, true)) {
                if(is_opponents_move_legal(piece, board_state,std::get<0>(square), std::get<1>(square))){
                    return false;
                }
            }
        }

    }

    return true; // Schachmatt
}

void Game::promote_pawn(std::shared_ptr<Chess_Piece>piece_ptr, char promotion_type) {
    piece_ptr->setPieceType(promotion_type);

    int8_t new_piece_value;
    switch (promotion_type) {
        case 'P': new_piece_value = 1; break; // Pawn
        case 'N': new_piece_value = 3; break; // Knight
        case 'B': new_piece_value = 4; break; // Bishop
        case 'R': new_piece_value = 5; break; // Rook
        case 'Q': new_piece_value = 9; break; // Queen
        case 'K': new_piece_value = 10; break; // King
        default: throw std::invalid_argument("Invalid Promotion Type");
    }
    int8_t color = (active_player > 0) ? 1 : -1;

    board_state[piece_ptr->get_row()][piece_ptr->get_col()] = new_piece_value * color;
}


std::vector<std::tuple<int, int>> Game::compute_block_squares_diag(
    const std::tuple<int, int>& defending_king_pos,
    int attacking_piece_row,
    int attacking_piece_col
) const{
    std::vector<std::tuple<int, int>> blockable_squares_diag;

    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);

    int diff_row =  king_row - attacking_piece_row;
    int diff_col =  king_col - attacking_piece_col;

    assert(abs(diff_row) == abs(diff_col) && "Error in compute diagonal block squares");

    int row_direction = (diff_row > 0) ? -1 : 1;
    int col_direction = (diff_col > 0) ? -1 : 1;

    int block_row = king_row + row_direction;
    int block_col = king_col + col_direction;

    while (block_row != attacking_piece_row && block_col != attacking_piece_col) {
        blockable_squares_diag.push_back({block_row, block_col});
        block_row += row_direction;
        block_col += col_direction;
    }
    return blockable_squares_diag;
}


std::vector<std::tuple<int, int>> Game::compute_block_squares_horizontal(
    const std::tuple<int, int>& defending_king_pos,
    int attacking_piece_row,
    int attacking_piece_col
) const{
    std::vector<std::tuple<int, int>> blockable_squares_horizontal;

    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);

    int diff_col =  king_col - attacking_piece_col;
    int col_direction = (diff_col > 0) ? -1 : 1;

    int block_row = king_row;
    int block_col = king_col + col_direction;

    while (block_col != attacking_piece_col) {
        blockable_squares_horizontal.push_back({block_row, block_col});
        block_col += col_direction;
    }
    return blockable_squares_horizontal;
}
std::vector<std::tuple<int, int>> Game::compute_block_squares_vertical(
    const std::tuple<int, int>& defending_king_pos,
    int attacking_piece_row,
    int attacking_piece_col
) const{
    std::vector<std::tuple<int, int>> blockable_squares_vertical;

    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);

    int diff_row =  king_row - attacking_piece_row;
    int row_direction = (diff_row > 0) ? -1 : 1;

    int block_row = king_row + row_direction;
    int block_col = king_col;

    while (block_row != attacking_piece_row) {
        blockable_squares_vertical.push_back({block_row, block_col});
        block_col += row_direction;
    }
    return blockable_squares_vertical;
}


void Game::print_board_state() const {
    const int cell_width = 3;

    for (int row = 7; row >= 0; --row) {
        std::cout << row +1<< " "; // Zeilennummer
        for (int col = 0; col < 8; ++col) {
            int piece = board_state[row][col];
            // Verwendung der valueToPiece Map für die Ausgabe
            auto it = valueToPiece.find(piece);
            if (it != valueToPiece.end()) {
                std::cout << std::setw(cell_width) << it->second << " ";
            } else {
                std::cout << std::setw(cell_width) << "??" << " "; // Fehlerfall, wenn der Wert nicht gefunden wird
            }
        }
        std::cout << "\n";
    }

    // Ausgabe der Spaltenbezeichner
    std::cout << "  ";
    for (char col = 'a'; col <= 'h'; ++col) {
        std::cout << std::setw(cell_width) << col << " ";
    }
    std::cout << "\n";
}

void Game::print_pieces_debug() {
    std::cout << "White Pieces:\n";
    for (const auto& piece : white_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row() << ", " << piece->get_col()<< ")\n";
    }

    // Debug-Ausgabe für schwarze Figuren
    std::cout << "Black Pieces:\n";
    for (const auto& piece : black_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row() << ", " << piece->get_col()<< ")\n";
    }

    std::cout << "Active Pieces:\n";
    for (const auto& piece : *active_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row() << ", " << piece->get_col()<< ")\n";
    }

    // Debug-Ausgabe für schwarze Figuren
    std::cout << "Passive Pieces:\n";
    for (const auto& piece : *passive_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row() << ", " << piece->get_col()<< ")\n";
    }
    std::cout << "\n";
}

std::shared_ptr<Chess_Piece> Game::create_piece(int val, int row_coord, int col_coord) {
    std::shared_ptr<Chess_Piece> piece;

    std::string color = (val > 0) ? "white" : "black";

    // Erstelle das Piece basierend auf dem Wert
    switch (std::abs(val)) {
        case 1:
            piece = std::make_shared<Chess_Piece>(row_coord, col_coord, color,'P');
        break;
        case 3:
            piece = std::make_shared<Chess_Piece>(row_coord, col_coord, color,'N');
        break;
        case 4:
            piece = std::make_shared<Chess_Piece>(row_coord, col_coord, color, 'B');
        break;
        case 5:
            piece = std::make_shared<Chess_Piece>(row_coord, col_coord, color, 'R');
        break;
        case 9:
            piece = std::make_shared<Chess_Piece>(row_coord, col_coord, color, 'Q');
        break;

        case 10: // King
            piece = std::make_shared<Chess_Piece>(row_coord, col_coord, color, 'K');
        break;
        case 0:
            break;
        // Weitere Fälle für andere Schachfiguren wie Rook, Knight, Bishop, King...
        default:
            throw std::invalid_argument("Unbekannter Wert für Schachfigur");
    }

    return piece;
}

void Game::print_history() {
    int move_number = 1;
    std::cout << "Game History: \n";
    for (size_t i = 0; i < game_history_str.size(); i += 2) {
        // Ausgabe der Zugnummer und des ersten Zugs
        std::cout << move_number << ". " << game_history_str[i];

        // Prüfen, ob es einen zweiten Zug für diesen Zug gibt
        if (i + 1 < game_history_str.size()) {
            std::cout << " " << game_history_str[i + 1];
        }

        // Zeilenumbruch nach jedem Zugpaar
        std::cout << std::endl;

        move_number++;
    }
}

bool Game::is_stalemate() {
    if(num_moves_played < 20) {
        return false;
    }

    auto defensive_pieces = (active_player > 0)  ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces) : std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);

    for (std::shared_ptr<Chess_Piece> piece : *defensive_pieces) {
        if (piece->getPieceType() =='Q') {
            for(int row = -1; row <= 1; row++) {
                for(int col = -1; col <= 1; col++) {
                    int new_queen_row = piece->get_row() + row;
                    int new_queen_col = piece->get_row() + col;
                    if(new_queen_row >= 0 && new_queen_row <8 && new_queen_col >= 0 && new_queen_row < 8) {
                        if(board_state[new_queen_row][new_queen_col] == 0) {
                            return false;
                        }
                    }
                }
            }
        }
    }

    int piece_owner = (active_player > 0) ? -1 : 1;
    std::shared_ptr<Chess_Piece> defending_king_ptr;
    for (const auto& piece : *defensive_pieces) {
        if (piece->getPieceType() == 'K'){
            defending_king_ptr = piece;
        }
    }

    for (const auto& piece : *defensive_pieces) {
        if (has_piece_available_move(piece, piece_owner)){
            return false;
        }
        return true;
    }
}


std::vector<std::shared_ptr<Move>> Game::get_available_moves(std::shared_ptr<Chess_Piece> piece, int piece_owner) {

    std::vector<std::shared_ptr<Move>> available_moves;
    std::vector<std::tuple<int, int>> coord_candidates = piece->get_available_coords_to_move_to(piece_owner, board_state);
    for (const auto& coord : coord_candidates) {
        int row = std::get<0>(coord);
        int col = std::get<1>(coord);
        auto move = std::make_shared<Move>(Move(true, piece->get_row(), piece->get_col(), row, col, "", (board_state[row][col] != 0), true, piece->getPieceType()));
        if (piece->getPieceType() != 'K' && is_opponents_move_legal(piece, board_state, row,col)) {
            available_moves.push_back(move);
        }
        else if(piece->getPieceType() == 'K' && is_opponents_king_move_legal(piece, board_state, row, col)) {
            available_moves.push_back(move);
        }
    }

    if(piece->getPieceType() == 'K') {
        if (check_castle('s')) {
            auto move = std::make_shared<Move>(Move(true, piece->get_row(), piece->get_col(), -1, -1, "short", false, false,'x'));
            available_moves.push_back(move);
        }
        if (check_castle('l')) {
            auto move = std::make_shared<Move>(Move(true, piece->get_row(), piece->get_col(), -1, -1, "long", false, false,'x'));
            available_moves.push_back(move);
        }
    }

    return available_moves;
}


bool Game::has_piece_available_move(std::shared_ptr<Chess_Piece> piece, int piece_owner) {
    std::vector<std::tuple<int, int>> coord_candidates = piece->get_available_coords_to_move_to(piece_owner, board_state);
    for (const auto& coord : coord_candidates) {
        int row = std::get<0>(coord);
        int col = std::get<1>(coord);
        auto move = std::make_shared<Move>(Move(true, piece->get_row(), piece->get_col(), row, col, "", (board_state[row][col] != 0), true, piece->getPieceType()));
        if (piece->getPieceType() != 'K' && is_opponents_move_legal(piece, board_state, row,col)) {
            return true;
        }
        if(piece->getPieceType() == 'K' && is_opponents_king_move_legal(piece, board_state, row, col)) {
            return true;
        }
    }
return false;
}

std::list<std::vector<std::shared_ptr<Move>>> Game::get_all_available_moves() {
    auto active_pieces = (active_player > 0) ? white_pieces : black_pieces;

    std::list<std::vector<std::shared_ptr<Move>>> available_moves;

    for (auto piece : active_pieces) {
        available_moves.push_back(get_available_moves(piece, active_player));
    }
    return available_moves;
}
