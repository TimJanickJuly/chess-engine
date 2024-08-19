
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

    std::vector<std::string> fools_mate_moves_white = {
        "e4", "e5",
        "Qf3", "c6",
        "Bc4", "a6",
        "Qxf7#",
    };

    std::vector<std::string> fools_mate_moves_black = {
        "a3", "e5",
        "a4", "Qf6",
        "a5", "Bc5",
        "a6", "Qxf2#"
    };

    std::vector<std::string> castle_moves_short = {
        "e4", "e5",
        "Nf3", "Nf6",
        "Bc4", "Bc5",
        "o-o", "o-o",
        "a3"

    };

    std::vector<std::string> simple_stalemate_moves = {
        "e4", "e5",
        "Nf3", "Nc6",
        "Bc4", "Bc5",
        "O-O", "O-O",
        "d3", "d6",
        "Bg5", "Nf6",
        "Nxe5", "dxe5",
        "Qf3", "Be6",
        "Rad1", "Rad8",
        "h3", "h6",
        "Bg4", "Bxg4",
        "Qxg4", "Qxg4",
        "Rxg4", "Rxg4",
        "Kf1", "Kf8"  // Patt entsteht hier
    };

    std::vector<std::string> stalemate_moves = {
        "e4", "e5",
        "f4", "f5",
        "Nf3", "Nc6",
        "fxe5", "Nxe5",
        "Nxe5", "fxe4",
        "Bc4", "Nf6",
        "d3", "exd3",
        "Qxd3", "d5",
        "Bxd5", "Bf5",
        "Qxf5", "Nxd5",
        "O-O", "Ba3",
        "bxa3", "Ne3",
        "Bxe3", "Qd1",
        "Rxd1", "Rd8",
        "Rxd8+", "Kxd8",
        "a4", "a5",
        "c4", "c5",
        "a3", "b5",
        "h3", "b4",
        "axb4", "g5",
        "b5", "h5",
        "Nd2", "g4",
        "Nf1", "g3",
        "Bg5+", "Kc7",
        "Rd1", "h4",
        "Ne3", "Kb7",
        "Bf4", "Ka7",
        "Nc2", "Rh7",
        "Rd7+", "Ka8",
        "Be3", "Rb7",
        "Na3", "Ka8",
        "Nf3", "Ka7",
        "Qd5", "Ka8",
        "Bg5", "Ka7",
        "Bd8", "Ka8",
        "Bb6", "Kb8",
        "Ne5", "Ka8",
        "Nd7"
    };

    std::vector<std::string> king_walk_test = {
        "e4", "e5",
            "Ke2", "Ke7",
            "Kd3", "Kd6",
            "Kc4", "Kc6",
            "h3", "Kb6",
            "Kd5", "c5",
            "Kxe5", "c4",
            "Kf5", "Kb5",
            "Qh5", "Kb4",
            "a3+", "Ka4",
            "Kf4","h6",
            "Nc3#"
    };


    int valid_move;

    for (const auto& move_input : stalemate_moves) {
        std::cout << "\n\n";
        board->print_history();
        std::cout << "\n";

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
            break;
        } else if (valid_move == -1) {
            std::cout << "Illegal move\n";
            break;
        }

        if (valid_move == 0) {
            std::cout << "Programm Stopped";
            break;
        }
        std::cout << "\n";
        print_board(*board);
    }
    std::cout << "\n";
    print_board(*board);

    return 0;
}






/*






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
*/