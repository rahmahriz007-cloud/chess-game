// ============================================================================
//  Types.h  -  Small shared types used by every class in the project
// ============================================================================
#ifndef TYPES_H
#define TYPES_H

#include <iosfwd>
#include <string>

enum class Color { White, Black };
enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn };

inline Color opposite(Color c) { return c == Color::White ? Color::Black : Color::White; }
inline const char* colorName(Color c) { return c == Color::White ? "White" : "Black"; }

// A square on the board.  row 0 = rank 1 (White's side), col 0 = file 'a'.
struct Position {
    int row;
    int col;

    Position(int r = -1, int c = -1) : row(r), col(c) {}

    bool isValid() const { return row >= 0 && row < 8 && col >= 0 && col < 8; }

    // Operator overloading: compare two squares naturally with == and !=
    bool operator==(const Position& o) const { return row == o.row && col == o.col; }
    bool operator!=(const Position& o) const { return !(*this == o); }

    std::string toString() const;                        // e.g. "e4"
    static Position fromString(const std::string& text); // "e4" -> Position, or invalid
};

// Operator overloading: lets us write  std::cout << position;
std::ostream& operator<<(std::ostream& os, const Position& p);

// A move from one square to another.  'promotion' is 0 unless a pawn promotes
// (then 'Q', 'R', 'B' or 'N').
struct Move {
    Position from;
    Position to;
    char promotion;

    Move(Position f = Position(), Position t = Position(), char promo = 0)
        : from(f), to(t), promotion(promo) {}
};

// What happened when a move was played (used for messages and tests).
struct MoveInfo {
    bool valid = false;
    bool capture = false;
    bool enPassant = false;
    bool castleKingSide = false;
    bool castleQueenSide = false;
    bool promotion = false;
    PieceType movedType = PieceType::Pawn;
    PieceType capturedType = PieceType::Pawn;
};

#endif // TYPES_H
