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
    //print_pieces_debug();

    std::cout << "White King Position: " << std::get<0>(white_king_pos) << ", " << std::get<1>(white_king_pos) << "\n";
    std::cout << "Black King Position: " << std::get<0>(black_king_pos) << ", " << std::get<1>(black_king_pos) << "\n";

    std::shared_ptr<Move> move = std::make_shared<Move>(Move::process_move_syntax(str_player_move));
    if (!move->is_legal_move) {
        return 1;
    }
    if (!consider_move(move)){
        return -1;
    }
        std::cout << "EVALUATING CHECKMATE\n\n\n";
        if(is_passive_player_in_check) {

            if (active_player > 0) {
                std::cout << "Black has been put under check\n";
            }else {
                std::cout << "White has been put under check\n";
            }
            if(is_checkmate()) {
                std::cout << "Checkmate Detected\n";
                return 0;
            }

            }
        else {
            if(is_stalemate()){
                std::cout << "Stalemate Detected\n";
                return 0;
            }
        }
        clean_up_after_turn();
        switchPlayer();
        game_history_str.push_back(str_player_move);
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
        std::cout << "Invalid Move Syntax\n";
        return false;
    }
    if (move->is_castling_move == "short" || move->is_castling_move == "long") {
        if (move->is_castling_move == "short") {
                if(check_castle('s')) {
                    execute_castle('s');
                    return true;
                }
                else {
                    return false;
                }

            }
        }
        if (move->is_castling_move == "long") {
                if(check_castle('l')) {
                    execute_castle('l');
                    return true;
                }
                else {
                    return false;
                }
            
        }
    

    std::vector<std::shared_ptr<Chess_Piece>> possible_movers;

    for (const auto& piece : *active_pieces) {
        if (piece->getPieceType() == move->piece_to_move) {
            if (piece->is_move_possible(board_state, move->row_target, move->col_target, move->is_capture, false)) {
                std::cout << "Found a piece of Type " << piece->getPieceType() <<" on (" << piece->get_row() << ", " << piece->get_col()<< ") that could move to target square\n";
                if (!is_king_in_check_after_move(piece, move, board_state, active_player)) {
                    std::cout << "The move passed the safety Test, own King is not in check\n";
                    possible_movers.push_back(piece);
                }
            }
        }
    }

    if (possible_movers.empty()) {
        std::cout << "Could not find a Piece that moves to that sqare!\n";
        return false;
    }
    
    if (possible_movers.size() > 1) {
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
    
    if (possible_movers.size() != 1) {
        std::cout << "Could not find a Piece that moves to that sqare!\n";
        return false;
    }
    
    std::shared_ptr<Chess_Piece> piece = possible_movers.at(0);

    
    execute_move(piece, move);
    auto passive_king_pos = (active_player > 0) ? black_king_pos : white_king_pos;

    is_active_player_in_check = false;
    std::cout << "Passive King Position: " << std::get<0>(passive_king_pos) << ", " << std::get<1>(passive_king_pos) << "\n";
    is_passive_player_in_check = is_square_attacked(passive_king_pos, active_pieces, board_state, false);
    return true;
}


bool Game::is_square_attacked(const std::tuple<int, int> &square,
                              std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> attacking_pieces,
                              const int8_t board_state[8][8], bool is_defense) const{

    int row = std::get<0>(square);
    int col = std::get<1>(square);
    std::cout << "Testing if Square " << row << ", " << col << " is attacked by any pieces\n";
    for (const auto &piece: *attacking_pieces) {
        if (!(piece->get_row() == std::get<0>(*capture_coords) && piece->get_col() == std::get<1>(*capture_coords))) {
            std::cout << "Found Piece " << piece->getPieceType() << ", " << piece->get_row() << ", " << piece->get_col() << ", Test if it can got to " << row << ", " << col << "\n";
            if (piece->is_move_possible(board_state, row, col, true, is_defense)) {
                std::cout << "It can reach the square\n";
                return true;
            }
            else {
                std::cout << "It cannot reach the square\n";
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

    // Setze die En Passant Option, falls es sich um einen Bauern handelt
    if (piece->getPieceType() == 'P') {
        int y_move = std::abs(move->get_col_CoordTarget() - piece->get_col());
        if (y_move == 2) {
            // Wenn der Bauer zwei Schritte vorwärts bewegt wurde, setze die En Passant Option
            set_enpassant_option(move->get_row_CoordTarget(), move->get_col_CoordTarget());
        }
    }

    // If a piece has to be captured, then remove it for the piece list
    if (move->is_capture) {
        std::vector<std::shared_ptr<Chess_Piece>>* opponent_pieces = (active_player > 0) ? &black_pieces : &white_pieces;

        auto it = std::find_if(opponent_pieces->begin(), opponent_pieces->end(), [&](const std::shared_ptr<Chess_Piece>& p) {
            return p->get_row() == move->get_row_CoordTarget() && p->get_col()== move->get_col_CoordTarget();
        });

        if (it != opponent_pieces->end()) {
            opponent_pieces->erase(it);
        }
    }
}

bool Game::is_king_in_check_after_move(
    const std::shared_ptr<Chess_Piece> piece_to_move,
    std::shared_ptr<Move> move,
    const int8_t board_state[8][8],
    int kings_color
) {

    std::cout << "\nAnalysing squares to run:\nSquare: " << move->get_row_CoordTarget() << ", " <<  move->get_col_CoordTarget();
    std::shared_ptr<std::vector<std::shared_ptr<Chess_Piece>>> attacking_pieces;

    if(kings_color == active_player) {
        attacking_pieces = (active_player > 0) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces) : std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);
    }

    else {
        attacking_pieces = (kings_color > 0) ? std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(black_pieces) : std::make_shared<std::vector<std::shared_ptr<Chess_Piece>>>(white_pieces);
    }


    auto defending_king_pos = (kings_color > 0)? white_king_pos : black_king_pos;
    std::cout << "\nactive player = " << active_player << " king_color " << kings_color << "\n";
    std::cout << "\nking currently on "  << std::get<0>(defending_king_pos) << ", " <<  std::get<1>(defending_king_pos) << "\n";

    //if(kings_color == active_player) {
        if (move->getPieceToMove() == 'K') {
            std::cout << "\nking is trying to move to "  << move->get_row_CoordTarget() << ", " <<  move->get_col_CoordTarget() << "\n";
            capture_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(move->row_target, move->col_target));
            bool result = is_square_attacked(std::make_tuple(move->row_target, move->get_col_CoordTarget()), attacking_pieces, board_state, true);
            capture_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(-1, -1));
            return result;
        }

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
            default: throw std::invalid_argument("Unbekannter Piece-Typ");
        }
        if (piece_to_move->getColor() == "black"){piece_value *= -1;}

        board_state_copy[start_row][start_col] = 0;
        board_state_copy[target_row][target_col] = piece_value;
        capture_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(target_row, target_col));
        bool result = is_square_attacked(defending_king_pos, attacking_pieces, board_state_copy, (active_player == kings_color));
        capture_coords = std::make_shared<std::tuple<int,int>>(std::make_tuple(0,0));

        return result;
    }
//}
void Game::clean_up_after_turn() {
    capture_coords = std::make_shared<std::tuple<int,int>>(-1,-1);
    en_passant_coords= std::make_shared<std::tuple<int,int>>(-1,-1);

    if (board_state[std::get<0>(white_king_pos)][std::get<1>(white_king_pos)] != 10) {
        std::cout << "Had to adjust white kings position\n";
        for (const auto& piece : white_pieces) {
            if(piece->getPieceType() == 'K') {
                white_king_pos = std::make_tuple(piece->get_row(), piece->get_col());
            }
        }
    }
    if (board_state[std::get<0>(black_king_pos)][std::get<1>(black_king_pos)] != -10) {
        std::cout << "Had to adjust black kings position\n";
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

    active_player = 1;

    white_king_pos = std::make_tuple(0, 4);
    black_king_pos = std::make_tuple(7, 4);
    
    is_active_player_in_check = false;
    is_passive_player_in_check = false;

    capture_coords = std::make_shared<std::tuple<int,int>>(-1,-1);
    en_passant_coords= std::make_shared<std::tuple<int,int>>(-1,-1);

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
            std::cout << "Error: König wurde bereits bewegt.\n";
            return false;
        }
        if (active_player_h_rook_moved) {
            std::cout << "Error: Turm auf der H-Linie wurde bereits bewegt.\n";
            return false;
        }
    }
    else if (castle_type == 'l') { // Lange Rochade
        if (active_player_has_king_moved) {
            std::cout << "Error: König wurde bereits bewegt.\n";
            return false;
        }
        if (active_player_a_rook_moved) {
            std::cout << "Error: Turm auf der A-Linie wurde bereits bewegt.\n";
            return false;
        }
    }


    int rook_row = (active_player > 0) ? 0 : 7;
    int rook_col = (castle_type == 's') ? 0 : 7;

    if (abs(board_state[rook_row][rook_col]) != 5) {
        return false;
    }
    if (abs(board_state[std::get<0>(active_king_pos)][std::get<1>(active_king_pos)]) != 10) {
        return false;
    }
    if (castle_type == 's') {

        for(int i = 0; i < 3; i++) {
            auto square_to_check = std::make_tuple(std::get<0>(active_king_pos), std::get<1>(active_king_pos) + i);
            if(is_square_attacked(square_to_check, passive_pieces, board_state, false)) {

                return false;
            }
            if(i > 0 && board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) { // check if squares are ampty but leave out kings and rooks square
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
    std::cout << "CASTLING IS POSSIBLE \n";
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
    auto passive_king_pos = (active_player > 0) ? black_king_pos : white_king_pos;
    int king_row = std::get<0>(passive_king_pos);
    int king_col = std::get<1>(passive_king_pos);

    std::cout << "IS KING HERE?? "<< king_row << " , " << king_col << "\n";
    
    std::vector<std::shared_ptr<Chess_Piece>> pieces_that_give_check;

    for (const auto& piece : *active_pieces) {
        if (piece->is_move_possible(board_state, king_row, king_col,true, false)) {
            std::cout << "Piece: " << piece->getPieceType() << " on " << piece->get_row() << ", " << piece->get_col() << " is giving check\n";
            pieces_that_give_check.push_back(piece);
        }
    }

    std::shared_ptr<Chess_Piece> defending_king_ptr;

    for (const auto& piece : *passive_pieces) {
        if (piece->get_row() == king_row && piece->get_col() == king_col) {
            defending_king_ptr = piece;
        }
    }

    bool multicheck = (pieces_that_give_check.size() > 1);

    // Try to move the king
    for (int row = -1; row <= 1; ++row) {
        for (int col = -1; col <= 1; ++col) {
            if (row != 0 || col != 0) {
                int new_row = king_row + row;
                int new_col = king_col + col;
                if (new_row >= 0 && new_row < 8 && new_col >= 0 && new_col < 8) {   // board constrains
                    if (board_state[new_row][new_col] == EE) {                       // if the square next to king is empty
                        if (defending_king_ptr->is_move_possible(board_state, new_row, new_col,true, false)) {
                            auto simulated_move = std::make_shared<Move>(true, defending_king_ptr->get_row(), defending_king_ptr->get_col(), new_row, new_col, "", false, false, 'K', false, false);
                            if(!is_king_in_check_after_move(defending_king_ptr, simulated_move, board_state, active_player * -1)) {
                                std::cout << "king can run " << new_row << ", " << new_col << ", " << board_state[new_row][new_col] << "\n";
                                return false; // King can run to safe square
                                }
                            else {
                                std::cout << "king can't run to " << new_row << ", " << new_col << ", " << board_state[new_row][new_col] << "because of check\n";
                            }
                        }else {
                            std::cout << "king can't run to " << new_row << ", " << new_col << ", " << board_state[new_row][new_col] << "\n";
                        }
                    }
                    // if the square next to the king is not empty
                    if (defending_king_ptr->is_move_possible(board_state, new_row, new_col,true, false)) {
                        auto simulated_move = std::make_shared<Move>(true, defending_king_ptr->get_row(), defending_king_ptr->get_col(), new_row, new_col, "", true, false, 'K', false, false);
                        if (!is_king_in_check_after_move(defending_king_ptr, simulated_move, board_state, active_player * -1)) {
                            std::cout << "king can fight\n";
                            return false; // King can capture an opposing piece
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
    std::shared_ptr<Chess_Piece> attacking_piece = pieces_that_give_check[0];
    for (const auto& piece : *passive_pieces) {
        if (piece->getPieceType() != 'K' && piece->is_move_possible(board_state, attacking_piece->get_row(), attacking_piece->get_col(), true, false) &&
            is_king_in_check_after_move(piece, std::make_shared<Move>(Move(attacking_piece->get_row(), attacking_piece->get_col(), true)), board_state, active_player * -1)) {
            std::cout << "No Checkmate, Attacker can be captured\n";
            std::cout << "By, " << piece->getPieceType() << ", on " << piece->get_row() << ", " << piece->get_col() << "\n";
            return false; // Eine Figur des Verteidigers kann die angreifende Figur schlagen
        }
    }

    // Try to Body-Block Check
    if (attacking_piece->getPieceType() == 'N' || attacking_piece->getPieceType() == 'P') {
        return true; // Checkmate, you cannot block checks by Knight or Pawn
    }

    std::vector<std::tuple<int, int>> blockable_squares;
    int col_diff = std::abs(king_col - attacking_piece->get_row());
    int row_diff = std::abs(king_row - attacking_piece->get_col());

    if (row_diff <= 1 && col_diff <= 1) {
        return true; // Checkmate, you cannot block an attacker that close to the king
    }

    std::vector<std::tuple<int, int>> possible_squares_for_blockers;

    if (is_attack_diagonal(passive_king_pos, attacking_piece)) {
        possible_squares_for_blockers= compute_block_squares_diag(passive_king_pos, attacking_piece->get_row(), attacking_piece->get_col(), blockable_squares);
    } else if (is_attack_vertical(passive_king_pos, attacking_piece)) {
        possible_squares_for_blockers = compute_block_squares_vertical(passive_king_pos, attacking_piece->get_row(), attacking_piece->get_col(), blockable_squares);
    } else if (is_attack_horizontal(passive_king_pos, attacking_piece)) {
        possible_squares_for_blockers = compute_block_squares_horizontal(passive_king_pos, attacking_piece->get_row(), attacking_piece->get_col(), blockable_squares);
    }


    for (const auto& square : blockable_squares) {
        for (const auto& piece : *passive_pieces) {
            if (piece->is_move_possible(board_state, std::get<0>(square), std::get<1>(square),true, false) &&
                is_king_in_check_after_move(piece, std::make_shared<Move>(Move(std::get<0>(square), std::get<1>(square), false)), board_state, active_player * -1)) {
                std::cout << "No Checkmate, attack can be blocked\n";
                return false;
            }
        }
    }

    return true; // Schachmatt
}


bool Game::is_attack_diagonal(std::tuple<int,int> defending_king_pos,std::shared_ptr<Chess_Piece> attacking_piece) const{
    if(attacking_piece->getPieceType() == 'Q' || attacking_piece->getPieceType() == 'B') {
        return(attacking_piece->get_row() == std::get<0>(defending_king_pos) || attacking_piece->get_col() == std::get<1>(defending_king_pos));
    }
    else {
        return false;
    }
}
bool Game::is_attack_vertical(std::tuple<int,int> defending_king_pos,std::shared_ptr<Chess_Piece> attacking_piece) const{
    return(attacking_piece->get_row() == std::get<0>(defending_king_pos));
}
bool Game::is_attack_horizontal(std::tuple<int,int> defending_king_pos,std::shared_ptr<Chess_Piece> attacking_piece) const{
    return(attacking_piece->get_col() == std::get<1>(defending_king_pos));
}

std::vector<std::tuple<int, int>> Game::compute_block_squares_diag(
    const std::tuple<int, int>& defending_king_pos,
    int attacking_piece_row,
    int attacking_piece_col,
    const std::vector<std::tuple<int, int>>& blockable_squares
) const{
    std::vector<std::tuple<int, int>> blockable_squares_diag;

    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);

    // Diagonale nach oben rechts
    for (int i = 1; king_row + i < 8 && king_col + i < 8; ++i) {
        blockable_squares_diag.push_back({king_row + i, king_col + i});
        if (king_row + i == attacking_piece_row && king_col + i == attacking_piece_col) break;
    }

    // Diagonale nach unten rechts
    for (int i = 1; king_row - i >= 0 && king_col + i < 8; ++i) {
        blockable_squares_diag.push_back({king_row - i, king_col + i});
        if (king_row - i == attacking_piece_row && king_col + i == attacking_piece_col) break;
    }

    // Diagonale nach oben links
    for (int i = 1; king_row + i < 8 && king_col - i >= 0; ++i) {
        blockable_squares_diag.push_back({king_row + i, king_col - i});
        if (king_row + i == attacking_piece_row && king_col - i == attacking_piece_col) break;
    }

    // Diagonale nach unten links
    for (int i = 1; king_row - i >= 0 && king_col - i >= 0; ++i) {
        blockable_squares_diag.push_back({king_row - i, king_col - i});
        if (king_row - i == attacking_piece_row && king_col - i == attacking_piece_col) break;
    }

    return blockable_squares_diag;
}


std::vector<std::tuple<int, int>> Game::compute_block_squares_vertical(
    const std::tuple<int, int>& defending_king_pos,
    int attacking_piece_row,
    int attacking_piece_col,
    const std::vector<std::tuple<int, int>>& blockable_squares
) const{
    std::vector<std::tuple<int, int>> blockable_squares_vertical;

    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);

    // Vertikal nach oben
    for (int i = king_row + 1; i < 8; ++i) {
        blockable_squares_vertical.push_back({i, king_col});
        if (i == attacking_piece_row && king_col == attacking_piece_col) break;
    }

    // Vertikal nach unten
    for (int i = king_row - 1; i >= 0; --i) {
        blockable_squares_vertical.push_back({i, king_col});
        if (i == attacking_piece_row && king_col == attacking_piece_col) break;
    }

    return blockable_squares_vertical;
}

std::vector<std::tuple<int, int>> Game::compute_block_squares_horizontal(
    const std::tuple<int, int>& defending_king_pos,
    int attacking_piece_row,
    int attacking_piece_col,
    const std::vector<std::tuple<int, int>>& blockable_squares
) const {
    std::vector<std::tuple<int, int>> blockable_squares_horizontal;
    
    int king_row = std::get<0>(defending_king_pos);
    int king_col = std::get<1>(defending_king_pos);

    // Horizontal nach rechts
    for (int i = king_col + 1; i < 8; ++i) {
        blockable_squares_horizontal.push_back({king_row, i});
        if (king_row == attacking_piece_row && i == attacking_piece_col) break;
    }

    // Horizontal nach links
    for (int i = king_col - 1; i >= 0; --i) {
        blockable_squares_horizontal.push_back({king_row, i});
        if (king_row == attacking_piece_row && i == attacking_piece_col) break;
    }

    return blockable_squares_horizontal;
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
    int piece_owner = (active_player > 0) ? -1 : 1;
    for (const auto& piece : *passive_pieces) {
        auto moves = get_available_moves(piece, piece_owner);
        if (moves.empty()) {
            std::cout << "For piece of type " << piece->getPieceType() << " were no moves found" << std::endl;
        } else {
            std::cout << "For piece of type " << piece->getPieceType() << " were moves found" << std::endl;
            return false;
        }
    }
    return true;
}

std::vector<std::shared_ptr<Move>> Game::get_available_moves(std::shared_ptr<Chess_Piece> piece, int piece_owner) {

    std::vector<std::shared_ptr<Move>> available_moves;
    std::vector<std::tuple<int, int>> koord_candidates = piece->get_available_coords_to_move_to(piece_owner, board_state);

    for (const auto& coord : koord_candidates) {
        int row = std::get<0>(coord);
        int col = std::get<1>(coord);
        auto move = std::make_shared<Move>(Move(true, piece->get_row(), piece->get_col(), row, col, "", (board_state[row][col] != 0), true, piece->getPieceType()));
        if (!is_king_in_check_after_move(piece, move, board_state, active_player * -1)) {
            available_moves.push_back(move);
        }
    }

    if (check_castle('s')) {
        auto move = std::make_shared<Move>(Move(true, -1, -1, -1, -1, "short", false, false,'x'));
        available_moves.push_back(move);
    }
    if (check_castle('l')) {
        auto move = std::make_shared<Move>(Move(true, -1, -1, -1, -1, "long", false, false,'x'));
        available_moves.push_back(move);
    }
    return available_moves;
}
