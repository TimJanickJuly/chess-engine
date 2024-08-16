#include <iostream>
#include <memory> // Für std::shared_ptr
#include "game.h"

void print_board(const Game& board) {
    board.print_board_state();
}

int main() {


    auto board = std::make_shared<Game>();
    std::string move_input;
    std::string game_mode;
    int valid_move;


    while (true) {
            std::cout << "\n\n";
            board->print_history();
            std::cout << "\n";
            print_board(*board);
        if (board->active_player ==  1) {
            std::cout << "White's Turn, Your move: \n";
        }
        else {
            std::cout << "Blacks's Turn, Your move: \n";
        }
        if (board->is_passive_player_in_check) {
            std::cout << "Your King is in check!\n";
        }
            std::getline(std::cin, move_input);

            valid_move = board->handle_turn(move_input);
            if (valid_move == 2) {
                std::cout << "\n";
            }
        if  (valid_move == 1) {
            std::cout << "Invalid Synatx\n";
        }
        if  (valid_move == -1) {
            std::cout << "Illegal move\n";
        }
            if (valid_move == 0)
                {
                std::cout << "Check and Mate\n";
                std::cout << "\n\n";
                board->print_history();
                std::cout << "\n";
                print_board(*board);
                break;
            }
        }

    //}


    return 0;
}
