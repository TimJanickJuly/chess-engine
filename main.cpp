#include <iostream>
#include <memory> // Für std::shared_ptr
#include "game.h"

void print_board(const Board& board) {
    board.print_board();
}

int main() {


    auto board = std::make_shared<Board>();
    std::string move_input;
    std::string game_mode;
    bool valid_move;

    //std::cout << board->player_turn << "Choose game mode: 'classic', 'blind'\n";
    //std::getline(std::cin, game_mode);
    //if (game_mode == "classic"){
    while (true) {
            print_board(*board);
        std::cout << "White King Check: " << (board->get_white_king_in_check() ? "Yes" : "No")
      << " , Black King Check: " << (board->get_black_king_in_check() ? "Yes" : "No")
      << "\n";
            std::cout << board->player_turn << "'s Turn, Your move: \n";
            std::getline(std::cin, move_input);
            valid_move = board->consider_move(move_input);

            if (valid_move) {
                // Wenn der Zug gültig war, wechsle den Spieler
                std::cout << "\n";
                board->player_turn = (board->player_turn == "black") ? "white" : "black";
            } else {
                // Wenn der Zug ungültig war, informiere den Spieler und fordere ihn auf, es erneut zu versuchen
                std::cout << "Try Again\n";

        }

        // Optional: Hier könnten Sie die Bedingung zum Beenden des Spiels hinzufügen
        // z.B. Abfrage, ob das Spiel beendet wurde (Schachmatt, Patt etc.)
    //}
        }

    return 0;
}
