#ifndef CHESS_PIECE_H
#define CHESS_PIECE_H

#include <string>

class Chess_Piece {
public:
    // Konstruktor
    Chess_Piece(int x_coord, int y_coord, const std::string& color)
        : x_coord(x_coord), y_coord(y_coord), color(color) {}

    // Getter-Methoden
    int get_row_Coord() const { return x_coord; }
    int get_col_Coord() const { return y_coord; }
    std::string getColor() const { return color; }

    // Setter-Methoden
    void set_row_coord(int x) { x_coord = x; }
    void set_col_Coord(int y) { y_coord = y; }
    void setColor(const std::string& c) { color = c; }

    virtual char getPieceType() const = 0;

    virtual ~Chess_Piece() = default;
    virtual bool is_move_possible(const int board[8][8], int target_coord_x, int target_coord_y, bool is_capture) const = 0;

private:
    int x_coord;       // X-Koordinate der Schachfigur (0-7)
    int y_coord;       // Y-Koordinate der Schachfigur (0-7)
    std::string color; // Farbe der Schachfigur ("black" oder "white")
};

#endif // CHESS_PIECE_H
