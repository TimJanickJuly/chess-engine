#include "Move.h"
#include <regex>
#include <set>
// Konstruktor
Move::Move(bool legal, int x_start, int y_start,
           int x_target, int y_target,
           std::string castling, bool capturing, bool difficult,
           char piece, bool check, bool mate)
    : is_legal_move(legal), col_start(x_start), row_start(y_start),
      row_target(x_target), col_target(y_target),
      is_castling_move(castling), capturing_move(capturing),
      difficult_move(difficult), piece_to_move(piece),
      is_check(check), is_mate(mate) {}

// Getter-Methoden
bool Move::getIsLegalMove() const { return is_legal_move; }
int Move::get_row_CoordStart() const { return col_start; }
int Move::get_col_CoordStart() const { return row_start; }
int Move::get_row_CoordTarget() const { return row_target; }
int Move::get_col_CoordTarget() const { return col_target; }
std::string Move::getIsCastlingMove() const { return is_castling_move; }
bool Move::getIsCapturingMove() const { return capturing_move; }
bool Move::getIsDifficultMove() const { return difficult_move; }
char Move::getPieceToMove() const { return piece_to_move; }
bool Move::getIsCheck() const { return is_check; }
bool Move::getIsMate() const { return is_mate; }

// Setter-Methoden
void Move::setIsLegalMove(bool legal) { is_legal_move = legal; }
void Move::set_row_CoordStart(int x_start) { col_start = x_start; }
void Move::set_col_CoordStart(int y_start) { row_start = y_start; }
void Move::set_row_CoordTarget(int x_target) { row_target = x_target; }
void Move::set_col_CoordTarget(int y_target) { col_target = y_target; }
void Move::setIsCastlingMove(std::string castling) { is_castling_move = castling; }
void Move::setIsCapturingMove(bool capturing) { capturing_move = capturing; }
void Move::setIsDifficultMove(bool difficult) { difficult_move = difficult; }
void Move::setPieceToMove(char piece) { piece_to_move = piece; }
void Move::setIsCheck(bool check) { is_check = check; }
void Move::setIsMate(bool mate) { is_mate = mate; }

// Print-Funktion (optional)
void Move::printMove() const {
    std::cout << "Legal Move: " << std::boolalpha << is_legal_move << "\n"
              << "Start Position: (" << col_start << ", " << row_start << ")\n"
              << "Target Position: (" << row_target << ", " << col_target << ")\n"
              << "Castling Move: " << is_castling_move << "\n"
              << "Capturing Move: " << is_capturing_move << "\n"
              << "Difficult Move: " << is_difficult_move << "\n"
              << "Piece to Move: " << piece_to_move << "\n"
              << "Check: " << is_check << "\n"
              << "Mate: " << is_mate << "\n";
}
std::string Move::get_algebraic_chess() const {

    if (!is_legal_move) {
        return "Tryed to print invalid Move";
    }

    if (getIsCastlingMove() == "short") {
        return "o-o";
    }
    if (getIsCastlingMove() == "long") {
        return "o-o-o";
    }

    std::string result;

    char char_to_write;
    bool write_switch;

    if (getPieceToMove() != 'P') {
        result += getPieceToMove();
    }
    
    if(get_row_CoordStart() != -1) {
        result += 'a' + col_start;
    }
    
    if(get_col_CoordStart() != -1) {
        result += '1' + row_start;
    }
    
    if(getIsCapturingMove()) {
        result += 'x';
    }
    
    char target_row = 'a' + row_target;
    char target_col = '1' + col_target;
    result += target_col;
    result += target_row;

    if (is_check) {
        result += '+';
    }
    if (is_mate) {
        result += '#';
    }

    return result;
}



std::unordered_map<char, int> Move::dict_col_coords = {
    {'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4},
    {'f', 5}, {'g', 6}, {'h', 7}
};
std::unordered_map<char, int> Move::dict_row_coords = {
    {'1', 0}, {'2', 1}, {'3', 2}, {'4', 3}, {'5', 4},
    {'6', 5}, {'7', 6}, {'8', 7}
};


void remove_x(std::string& str) {
    std::string result;
    for (char ch : str) {
        if (ch != 'x') {
            result += ch;
        }
    }
    str = result;
}

bool Move::is_difficult_move(const std::string& move) {
    int digit_count = 0;
    int letter_count = 0;

    for(char c : move) {
        if (isdigit(c)) {
            digit_count++;
        } else if (c >= 'a' && c <= 'h') {
            letter_count++;
        }

        // Wenn beide Bedingungen erfüllt sind, können wir sofort true zurückgeben.
        if (digit_count >= 2 || letter_count >= 2) {
            return true;
        }
    }

    return false;
}

bool Move::is_capturing_move(const std::string& move) {
    return move.find('x') != std::string::npos;
}


bool Move::is_legal_syntax_move(const std::string& input) {
        std::string move_pattern = R"([KQRBNP]?(([a-h]|[1-8])|[a-h][1-8])?[x]?[a-h][1-8][#]?[+]?)";
    std::string castling_pattern = R"((O-O|O-O-O|o-o|o-o-o)$)";
    std::regex move_re(move_pattern);
    std::regex castling_re(castling_pattern);
    return std::regex_match(input, move_re) || std::regex_match(input, castling_re);
}

bool Move::is_piece_move(const std::string& player_move) {
    std::string piece_names = "RNBQK";
    return piece_names.find(player_move[0]) != std::string::npos;
}




Move Move::process_move_syntax(std::string& str_player_move) {
    Move move = Move();

    if (str_player_move.empty() || str_player_move.length() >= 7) {
        return move;
    }

    if (!is_legal_syntax_move(str_player_move)) {
        return move;
    }

    if (str_player_move == "o-o" || str_player_move == "O-O") {
        move.setIsCastlingMove("short");
        move.setIsLegalMove(true);
        return move;
    }
    if (str_player_move == "o-o-o" || str_player_move == "O-O-O") {
        move.setIsCastlingMove("long");
        move.setIsLegalMove(true);
        return move;
    }

    if (str_player_move.back() == '#' || str_player_move.back() == '+') {
        if (str_player_move.back() == '#') {
            move.setIsMate(true);
        }
        else {
            move.setIsCheck(true);
        }
        str_player_move.pop_back();
    }



    if (is_difficult_move(str_player_move)) {
        move.setIsDifficultMove(true);
    }

    if (str_player_move.find('x') != std::string::npos) {
        move.setIsCapturingMove(true);
    }

    if(is_piece_move(str_player_move)) {
        char piece;
        move.setPieceToMove(str_player_move[0]);
        str_player_move.erase(0,1);
    }
    else {
        move.setPieceToMove('P');
    }


    

    if(!move.getIsDifficultMove()){
        if(str_player_move[0] == 'x') {
            str_player_move.erase(0,1);
        }



        move.set_col_CoordTarget(dict_col_coords.at(str_player_move[0]));
        move.set_row_CoordTarget(dict_row_coords.at(str_player_move[1]));


    }


    else if(move.getIsDifficultMove()) {
        static const std::set<char> valid_chars = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
        static const std::set<char> valid_digits = {'1', '2', '3', '4', '5', '6', '7', '8'};
        if(valid_chars.find(str_player_move[0]) != valid_chars.end())  // wenn "a,b, .., h" gelesen wird
        {
            move.set_col_CoordStart(dict_col_coords.at(str_player_move[0]));
            str_player_move.erase(0,1);


        }
        if(valid_digits.find(str_player_move[0]) != valid_digits.end())  // wenn "1,2, ..., 8" gelesen wird
        {
            move.set_row_CoordStart(dict_row_coords.at(str_player_move[0]));
            str_player_move.erase(0,1);


        }
        if(str_player_move[0] == 'x') {
            str_player_move.erase(0,1);


        }
        move.set_col_CoordTarget(dict_col_coords.at(str_player_move[0]));

        move.set_row_CoordTarget(dict_row_coords.at(str_player_move[1]));
        
    }
    move.setIsLegalMove(true);
    return move;
}
