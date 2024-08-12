#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <unordered_map>

class Move {
public:
    // Konstruktor mit neuen Feldern
    Move(bool legal = false, int x_start = -1, int y_start = -1,
         int x_target = 0, int y_target = 0,
         std::string castling = "None", bool capturing = false, bool difficult = false,
         char piece = 'X', bool check = false, bool mate = false);

    // Getter-Methoden
    bool getIsLegalMove() const;
    int get_row_CoordStart() const;
    int get_col_CoordStart() const;
    int get_row_CoordTarget() const;
    int get_col_CoordTarget() const;
    std::string getIsCastlingMove() const;
    bool getIsCapturingMove() const;
    bool getIsDifficultMove() const;
    char getPieceToMove() const;
    bool getIsCheck() const;
    bool getIsMate() const;

    // Setter-Methoden
    void setIsLegalMove(bool legal);
    void set_row_CoordStart(int x_start);
    void set_col_CoordStart(int y_start);
    void set_row_CoordTarget(int x_target);
    void set_col_CoordTarget(int y_target);
    void setIsCastlingMove(std::string castling);
    void setIsCapturingMove(bool capturing);
    void setIsDifficultMove(bool difficult);
    void setPieceToMove(char piece);
    void setIsCheck(bool check);
    void setIsMate(bool mate);

    void printMove() const;

    static std::unordered_map<char, int> dict_row_coords;
    static std::unordered_map<char, int> dict_col_coords;
    std::string get_algebraic_chess() const;


    static Move process_move_syntax(std::string& str_player_move);
    //private:
    static bool is_difficult_move(const std::string& input);
    static bool is_capturing_move(const std::string& move);
    static bool is_legal_syntax_move(const std::string& input);
    static bool is_piece_move(const std::string& player_move);
    void make_move(char piece, int x_to, int y_to, int x_from, int y_from);


public:
    bool is_legal_move;
    int row_start;
    int col_start;
    int row_target;
    int col_target;
    std::string is_castling_move;
    bool capturing_move;
    bool difficult_move;
    char piece_to_move;
    bool is_check; // Neues Feld
    bool is_mate;  // Neues Feld
};

#endif // MOVE_H
