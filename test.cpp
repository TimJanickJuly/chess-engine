#include <iostream>
#include <memory> // Für std::shared_ptr
#include <vector>
#include "game.h"
#include <iostream>
#include <memory> // Für std::shared_ptr


void print_board(const Game& board) {
    board.print_board_state();
}

int main() {
    auto board = std::make_shared<Game>();

    // Liste der Züge im Standard-Schachnotation (z.B. "e2e4" für Bauer von e2 nach e4)
    std::vector<std::string> moves = {
        "e2e4", "e7e5",
        "g1f3", "b8c6",
        "f1c4", "g8f6",
        // Weitere Züge können hier hinzugefügt werden
    };

    int valid_move;

    for (const auto& move_input : moves) {
        std::cout << "\n\n";
        board->print_history();
        std::cout << "\n";
        print_board(*board);

        if (board->active_player == 1) {
            std::cout << "White's Turn, Move: " << move_input << "\n";
        } else {
            std::cout << "Black's Turn, Move: " << move_input << "\n";
        }

        if (board->is_passive_player_in_check) {
            std::cout << "Your King is in check!\n";
        }

        valid_move = board->handle_turn(move_input);

        if (valid_move == 2) {
            std::cout << "\n";
        } else if (valid_move == 1) {
            std::cout << "Invalid Syntax\n";
        } else if (valid_move == -1) {
            std::cout << "Illegal move\n";
        }

        if (valid_move == 0) {
            std::cout << "Check and Mate\n";
            std::cout << "\n\n";
            board->print_history();
            std::cout << "\n";
            print_board(*board);
            break;
        }
    }

    return 0;
}
