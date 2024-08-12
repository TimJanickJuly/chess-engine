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

#include "Bishop.h"
#include "Chess_Piece.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"

std::unordered_map<int, std::string> Board::valueToPiece = {
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

std::unordered_map<std::string, int> Board::pieceToValue = {
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


// TODO Implement a list of Possible Moves, and a function thta calculates them
// Oder versuche Checkmate effizienter zu implementieren

std::shared_ptr<Chess_Piece> Board::create_piece(int val, int row_coord, int col_coord) {
    std::shared_ptr<Chess_Piece> piece;

    std::string color = (val > 0) ? "white" : "black";

    // Erstelle das Piece basierend auf dem Wert
    switch (std::abs(val)) {
        case 1:
            piece = std::make_shared<Pawn>(row_coord, col_coord, color);
        break;
        case 3:
            piece = std::make_shared<Knight>(row_coord, col_coord, color);
        break;
        case 4:
            piece = std::make_shared<Bishop>(row_coord, col_coord, color);
        break;
        case 5:
            piece = std::make_shared<Rook>(row_coord, col_coord, color);
        break;
        case 9:
            piece = std::make_shared<Queen>(row_coord, col_coord, color);
        break;

        case 10: // King
            piece = std::make_shared<King>(row_coord, col_coord, color);
        break;
        case 0:
            break;
        // Weitere Fälle für andere Schachfiguren wie Rook, Knight, Bishop, King...
        default:
            throw std::invalid_argument("Unbekannter Wert für Schachfigur");
    }

    return piece;
}
bool Board::consider_move(std::string& str_player_move) {
    Move move = Move::process_move_syntax(str_player_move);
    move.printMove();

    if (!move.is_legal_move) {
        std::cout << "Invalid Move Syntax\n";
        return false;
    }
    if (move.is_castling_move == "short" || move.is_castling_move == "long") {
        bool current_player_has_king_moved = (player_turn == "white") ? has_white_king_moved : has_black_king_moved;
        bool current_player_a_rook_moved = (player_turn == "white") ? has_white_a_rook_moved : has_black_a_rook_moved;
        bool current_player_h_rook_moved = (player_turn == "white") ? has_white_h_rook_moved : has_black_h_rook_moved;
        if (move.is_castling_move == "short") {
            if (!current_player_has_king_moved && !current_player_h_rook_moved) {
                if(check_castle(player_turn, "short")) {
                    execute_castle(player_turn, "short");
                }
                else {
                    return false;
                }

            }
        }
        if (move.is_castling_move == "long") {
            if (!current_player_has_king_moved && !current_player_a_rook_moved){
                if(check_castle(player_turn, "long")) {
                    execute_castle(player_turn, "long");
                }
                else {
                    return false;
                }
            }
        }
    }

    std::vector<std::shared_ptr<Chess_Piece>> possible_movers = find_moving_piece(move);

    if (possible_movers.empty()) {
        std::cout << "Could not find a Piece that moves to that sqare!\n";
        return false;
    }

    if (possible_movers.size() > 1) {
        for (auto it = possible_movers.begin(); it != possible_movers.end(); ) {
            const auto& piece = *it;
            if ((move.get_row_CoordStart() != -1 && piece->get_row_Coord() != move.get_row_CoordStart()) ||
                (move.get_col_CoordStart() != -1 && piece->get_col_Coord() != move.get_col_CoordStart())) {
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

    std::shared_ptr<Chess_Piece> piece_ptr = possible_movers.at(0);
    Chess_Piece& piece = *piece_ptr;

    execute_move(piece, move.get_row_CoordTarget(), move.get_col_CoordTarget(), move.is_capturing_move);
    auto opponent_king_position = (player_turn == "white")? black_king_pos: white_king_pos;
    auto own_pieces = (player_turn == "white")? white_pieces:black_pieces;
    std::cout << "---\n";

    if(player_turn == "white") {
        set_white_king_in_check(false);
    }
    else {
        set_white_king_in_check(false);
    }

    if(test_for_check(opponent_king_position, own_pieces, board_state)) {
        if(player_turn == "white") {
            set_black_king_in_check(true);
        }
        else {
            set_white_king_in_check(true);
        }
    }


    return true;
}

std::vector<std::shared_ptr<Chess_Piece>> Board::find_moving_piece(const Move& move) {

    std::vector<std::shared_ptr<Chess_Piece>>* pieces_own = nullptr;
    std::vector<std::shared_ptr<Chess_Piece>>* pieces_opponent = nullptr;

    std::string current_player = player_turn;

    if (current_player == "white") {
        pieces_own = &white_pieces;
        pieces_opponent = &black_pieces;
    } else {
        pieces_own = &black_pieces;
        pieces_opponent = &white_pieces;
    }


    std::vector<std::shared_ptr<Chess_Piece>> possible_movers;

    for (const auto& piece : *pieces_own) {
        if (piece->getPieceType() == move.piece_to_move) {
            if (piece->is_move_possible(board_state, move.row_target, move.col_target, move.capturing_move)) {
                std::cout << "Found a piece on (" << piece->get_row_Coord() << ", " << piece->get_col_Coord() << ") that could move to target square\n";
                if (test_check_simulation(piece, move, board_state, pieces_opponent, move.getIsCapturingMove())) {
                    std::cout << "The Move passed the Check Simulation Test\n";
                    possible_movers.push_back(piece);
                }
                else {
                    std::cout << "The Move did not pass the Check Simulation Test\n";
                }
            }
        }
    }

    return possible_movers;
}


bool Board::test_for_check(const std::tuple<int, int>& king_position,
                           const std::vector<std::shared_ptr<Chess_Piece>>& piece_list,
                           const int board[8][8]) const {
    int row_pos_king = std::get<0>(king_position);
    int col_pos_king = std::get<1>(king_position);

    for (const auto& piece : piece_list) {
        if (piece->is_move_possible(board, row_pos_king, col_pos_king, true)) {
            std::cout << piece->getPieceType() << ", " << piece->get_row_Coord() << ", " << piece->get_col_Coord() << "\n";
            return true;
        }
    }
    return false;
}

void Board::execute_move(Chess_Piece& piece, int target_coord_row, int target_coord_col, bool is_capture) {
    // Setze die En Passant Option auf -1, -1, wenn es sich nicht um einen Bauern handelt oder keine En Passant-Bedingung erfüllt ist
    set_enpassant_option(-1, -1);

    // Hole die aktuelle Position der Figur
    int start_coord_row = piece.get_row_Coord();
    int start_coord_col = piece.get_col_Coord();

    // Bestimme den Wert, der das Schachbrett repräsentiert
    int piece_value = 0;
    switch (piece.getPieceType()) {
        case 'P': piece_value = 1; break; // Pawn
        case 'N': piece_value = 3; break; // Knight
        case 'B': piece_value = 4; break; // Bishop
        case 'R': piece_value = 5; break; // Rook
        case 'Q': piece_value = 9; break; // Queen
        case 'K': piece_value = 10; break; // King
        default: throw std::invalid_argument("Unbekannter Figurtyp");
    }

    if (piece.getColor() == "black") {
        piece_value *= -1; // Negative Werte für schwarze Figuren
    }

    // Setze die Zielposition auf die Figur
    board_state[target_coord_row][target_coord_col] = piece_value;

    // Leere die Startposition
    board_state[start_coord_row][start_coord_col] = EE; // Annahme, dass EE den Wert für ein leeres Feld darstellt

    // Aktualisiere die Position der Figur
    piece.set_row_coord(target_coord_row);
    piece.set_col_Coord(target_coord_col);

    // Aktualisiere die Position der Könige, falls es sich um einen König handelt
    if (piece.getPieceType() == 'K') {
        if (piece.getColor() == "white") {
            white_king_pos = std::make_tuple(target_coord_row, target_coord_col);
        } else {
            black_king_pos = std::make_tuple(target_coord_row, target_coord_col);
        }
    }

    // Setze die En Passant Option, falls es sich um einen Bauern handelt
    if (piece.getPieceType() == 'P') {
        int y_move = std::abs(target_coord_col - start_coord_col);
        if (y_move == 2) {
            // Wenn der Bauer zwei Schritte vorwärts bewegt wurde, setze die En Passant Option
            set_enpassant_option(target_coord_row, target_coord_col);
        }
    }

    // Entferne die gefangene Figur, wenn es sich um einen Fangzug handelt
    if (is_capture) {
        std::vector<std::shared_ptr<Chess_Piece>>* opponent_pieces = (piece.getColor() == "white") ? &black_pieces : &white_pieces;

        auto it = std::find_if(opponent_pieces->begin(), opponent_pieces->end(), [&](const std::shared_ptr<Chess_Piece>& p) {
            return p->get_row_Coord() == target_coord_row && p->get_col_Coord() == target_coord_col;
        });

        if (it != opponent_pieces->end()) {
            opponent_pieces->erase(it);
        }
    }
}

bool Board::test_check_simulation(
    const std::shared_ptr<Chess_Piece>& piece,
    const Move& move,
    const int board_state[8][8],
    const std::vector<std::shared_ptr<Chess_Piece>>* pieces_opponent, bool is_capture
) const {

    int board_copy[8][8];
    memcpy(board_copy, board_state, sizeof(board_copy));

    std::vector<std::shared_ptr<Chess_Piece>> opponent_pieces_copy = *pieces_opponent;

    int target_row = move.get_row_CoordTarget();
    int target_col = move.get_col_CoordTarget();

    if(is_capture) {
        auto it = std::remove_if(opponent_pieces_copy.begin(), opponent_pieces_copy.end(),
            [target_row, target_col](const std::shared_ptr<Chess_Piece>& opp_piece) {
                return (opp_piece->get_row_Coord() == target_row && opp_piece->get_col_Coord() == target_col);
            });

        if (it != opponent_pieces_copy.end()) {
            opponent_pieces_copy.erase(it, opponent_pieces_copy.end());
        }
    }

    int start_row = piece->get_row_Coord();
    int start_col = piece->get_col_Coord();
    int piece_value;

    switch (abs(piece->getPieceType())) {
        case 'P': piece_value = 1; break;
        case 'N': piece_value = 3; break;
        case 'B': piece_value = 4; break;
        case 'R': piece_value = 5; break;
        case 'Q': piece_value = 9; break;
        case 'K': piece_value = 10; break;
        default: throw std::invalid_argument("Unbekannter Piece-Typ");
    }
    if (piece->getColor() == "black"){piece_value *= -1;}


    board_copy[start_row][start_col] = 0;
    board_copy[target_row][target_col] = piece_value;

    const std::tuple<int, int>& king_position = (piece->getColor() == "white") ? white_king_pos : black_king_pos;

    return !test_for_check(king_position, opponent_pieces_copy, board_copy);
}

/*
bool Board::test_mate(std::string current_player) {
    auto& list_pieces_attacker = (current_player == "white") ? white_pieces : black_pieces;
    auto& list_pieces_defender = (current_player == "white") ? black_pieces : white_pieces;
    auto king_pos_defender = (current_player == "white") ? white_king_pos : black_king_pos;
    int direction = (current_player == "white") ? 1 : -1;
    Move move();

    for (const auto& defendig_piece : list_pieces_defender) {
        int defending_piece_row = defendig_piece->get_row_Coord();
        int defending_piece_col = defendig_piece->get_col_Coord();
        Move move = Move::Move();

        if (defendig_piece->getPieceType() == 'P') {
            move.setPieceToMove('P');
            move.set_row_CoordTarget();
            move.set_col_CoordTarget()
            test_check_simulation(defendig_piece, )
        }
    }

}
*/

Board::Board() {

    player_turn = "white";
    set_enpassant_option(-1, -1);

    white_king_pos = std::make_tuple(0, 4);
    black_king_pos = std::make_tuple(7, 4);

    // Initialize new fields
    has_black_king_moved = false;
    has_white_king_moved = false;
    has_white_a_rook_moved = false;
    has_white_h_rook_moved = false;
    has_black_a_rook_moved = false;
    has_black_h_rook_moved = false;

    // Die Vektoren `white_pieces` und `black_pieces` sind bereits als Member der Klasse vorhanden,
    // daher müssen sie hier nicht noch einmal lokal deklariert werden.

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

    // Debug-Ausgabe für weiße Figuren
    std::cout << "White Pieces:\n";
    for (const auto& piece : white_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row_Coord() << ", " << piece->get_col_Coord() << ")\n";
        std::cout << "Confirm Position on Board " << board_state[piece->get_row_Coord()][piece->get_col_Coord()] << "\n";
    }

    // Debug-Ausgabe für schwarze Figuren
    std::cout << "Black Pieces:\n";
    for (const auto& piece : black_pieces) {
        std::cout << "Type: " << piece->getPieceType() << ", Color: " << piece->getColor()
                  << ", Position: (" << piece->get_row_Coord() << ", " << piece->get_col_Coord() << ")\n";
        std::cout << "Confirm Position on Board " << board_state[piece->get_row_Coord()][piece->get_col_Coord()] << "\n";
    }
}

bool Board::check_castle(std::string current_player, std::string castle_type) {
    auto opponents_piece = (current_player == "white") ? black_pieces : white_pieces;
    auto king_pos =  (current_player == "white") ? white_king_pos : black_king_pos;
    int rook_row = (current_player == "white") ? 0 : 7;
    int rook_col = (castle_type == "short") ? 7 : 0;

    if (abs(board_state[rook_row][rook_col]) != 5) {
        return false;
    }
    if (abs(board_state[std::get<0>(king_pos)][std::get<1>(king_pos)]) != 10) {
        return false;
    }
    if (castle_type == "short") {
        std::tuple<int,int> square_to_check = std::make_tuple(rook_row, 4);
        if(test_for_check(square_to_check, opponents_piece, board_state) ){
            return false;
        }
        square_to_check = std::make_tuple(rook_row, 5);
        if(test_for_check(square_to_check, opponents_piece, board_state)|| board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) {
            return false;
        }
        square_to_check = std::make_tuple(rook_row, 6);
        if(test_for_check(square_to_check, opponents_piece, board_state)|| board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) {
            return false;
        }
    }
    if (castle_type == "long") {
        std::tuple<int,int> square_to_check = std::make_tuple(rook_row, 4);
        if(test_for_check(square_to_check, opponents_piece, board_state) ){
            return false;
        }
        square_to_check = std::make_tuple(rook_row, 3);
        if(test_for_check(square_to_check, opponents_piece, board_state)|| board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) {
            return false;
        }
        square_to_check = std::make_tuple(rook_row, 2);
        if(test_for_check(square_to_check, opponents_piece, board_state)|| board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) {
            return false;
        }
        square_to_check = std::make_tuple(rook_row, 1);
        if(test_for_check(square_to_check, opponents_piece, board_state)|| board_state[std::get<0>(square_to_check)][std::get<1>(square_to_check)] != 0) {
            return false;
        }
    }
    return true;
}

void Board::execute_castle(std::string current_player, std::string castle_type) {
    // Kopiere die Liste der Figuren in den Vektor list_pieces
    auto& list_pieces = (current_player == "white") ? white_pieces : black_pieces;
    auto king_pos = (current_player == "white") ? white_king_pos : black_king_pos;
    int rook_row = (current_player == "white") ? 0 : 7;
    int rook_col = (castle_type == "short") ? 7 : 0;
    std::shared_ptr<Chess_Piece> king;
    std::shared_ptr<Chess_Piece> rook;

    // Finde den Turm
    auto it_rook = std::find_if(list_pieces.begin(), list_pieces.end(),
                                [=](const std::shared_ptr<Chess_Piece>& piece) {
                                    return piece->getPieceType() == 'R' &&
                                           piece->get_row_Coord() == rook_row &&
                                           piece->get_col_Coord() == rook_col;
                                });

    if (it_rook != list_pieces.end()) {
        rook = *it_rook;
    } else {
        std::cerr << "Rook not found for castling!" << std::endl;
        return; // Abbrechen, wenn der Turm nicht gefunden wird
    }

    // Finde den König
    auto it_king = std::find_if(list_pieces.begin(), list_pieces.end(),
                                [=](const std::shared_ptr<Chess_Piece>& piece) {
                                    return piece->getPieceType() == 'K' &&
                                           piece->get_row_Coord() == std::get<0>(king_pos) &&
                                           piece->get_col_Coord() == std::get<1>(king_pos);
                                });

    if (it_king != list_pieces.end()) {
        king = *it_king;
    } else {
        std::cerr << "King not found for castling!" << std::endl;
        return; // Abbrechen, wenn der König nicht gefunden wird
    }

    if (castle_type == "short") {
        king->set_col_Coord(6);
        rook->set_col_Coord(5);
        board_state[rook_row][4] = 0; // Ursprüngliche Position des Königs leeren
        board_state[rook_row][7] = 0; // Ursprüngliche Position des Turms leeren
        board_state[rook_row][5] = (current_player == "white") ? 5 : -5;
        board_state[rook_row][6] = (current_player == "white") ? 10 : -10;

    } else if (castle_type == "long") {
        king->set_col_Coord(2);
        rook->set_col_Coord(3);
        board_state[rook_row][4] = 0; // Ursprüngliche Position des Königs leeren
        board_state[rook_row][0] = 0; // Ursprüngliche Position des Turms leeren
        board_state[rook_row][3] = (current_player == "white") ? 5 : -5;
        board_state[rook_row][2] = (current_player == "white") ? 10 : -10;
    }
}








void Board::print_board() const {
    // Setze die Breite für die Felder und Bezeichner
    const int cell_width = 3;

    // Ausgabe der Zeilen des Schachbretts
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
