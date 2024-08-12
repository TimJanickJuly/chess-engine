/*

#include "util.h"
#include "algo.h"
#include "rule.h"
#include "test_board.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>

#define BLACK -1
#define WHITE 1

#define PLAYER WHITE
#define AI BLACK


using namespace std;

bool check_move_legal(str_player_input, board) {
    board[0]
}


int main() {
    std::string game_state = "running";
    int board[8][8] = {
        {BR, BN, BB, BQ, BK, BB, BN, BR},
        {BP, BP, BP, BP, BP, BP, BP, BP},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {EE, EE, EE, EE, EE, EE, EE, EE},
        {WP, WP, WP, WP, WP, WP, WP, WP},
        {WR, WN, WB, WQ, WK, WB, WN, WR}
    };
    bool passed = 0;
    std::string user_input;

    while (passed == 0){
    std::cout << "White or Black ";
    std::cin >> user_input;
        if (user_input == "White" or user_input == "Black")
        {
            passed = 1;
        }
        else {
            std::cout << "Try Again";
        }
    }

    string str_player_side = (user_input == "White") ? "WHITE":"BLACK";
    string str_ai_side = (user_input == "White") ? "BLACK":"WHITE";


    cout << "\n\n"
         << "**********************\n"
         << " WELCOME TO CHESS A.I.\n"
         << "======================\n"
         << "=    BLACK upside    =\n"
         << "=    WHITE dnside    =\n"
         << "=--------------------=\n"
         << "=    PLAYER " << str_player_side << "    =\n"
         << "=      A.I. " << str_ai_side << "    =\n"
         << "======================\n\n\n";
    cout << "You are ";
    cout << user_input;
    cout << "\n\n";

    // start playing
    bool is_player_move = (str_player_side == "WHITE");


    while (game_state == "running") {
        if (is_player_move) {
            bool move_valid = false;
            while (!move_valid) {
                std::string str_player_move;
                cout << " ____________\n"
                     << " It's your turn. Make a move.\\\n";
                cin >> str_player_move;
                if (cin.fail()) {
                    cout << "Try again";
                    cin.clear();
                    cin.ignore();
                }
                cin.ignore(100000, '\n');
                cout << "\\_____________________________________________/\n";
                if (check_move_legal(str_player_move, board)
                {
                    move_valid = true;
                } else {
                    cout <<"*******  CANNOT MOVE  ********\n";
                }
            }
        } else {
            clock_t start = clock();
            cout << "\n    A.I is thinking ... \n\n";
            int ai_side = (PLAYER == BLACK) ? WHITE:BLACK;
            // no need to check validity here
            ai_move(&bs);
            ai_single_move_duration = (clock() - start) / (double) CLOCKS_PER_SEC;
            print_board_move(bs, AI, ai_next_start_r, ai_next_start_c, ai_next_end_r, ai_next_end_c);
        }
        print_check(bs);
        is_player_move = !is_player_move;
    }

    // print result
    for (int r = 0; r <= 7; r++) {
        for (int c = 0; c <= 7; c++) {
            if (abs(bs.get_piece(r,c)) != KING)
                continue;
            int win_side = (bs.get_piece(r,c)*AI > 0) ? AI : PLAYER;
            if (win_side == PLAYER)
                cout << "\n\n    YOU WIN !!!\n";
            else
                cout << "\n\n    YOU LOST !!!\n";
            break;
        }
    }
}
*/