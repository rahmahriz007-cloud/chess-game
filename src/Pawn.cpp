// Pawn.cpp
//   [Insia Fatima]   constructor, destructor, clone, name, value
//   [Rahmah Rizwan]  isValidMove() / attacksSquare() overrides:
//                    1-step, 2-step, diagonal capture, en passant
#include "Pawn.h"
#include "Board.h"
#include <cstdlib>

// ===== [Insia] Constructor / destructor / attributes =========================
Pawn::Pawn(Color color) : Piece(color, PieceType::Pawn) {
    Piece::traceMessage("[ctor ] Pawn (derived) constructor finished");
}
Pawn::~Pawn() { Piece::traceMessage("[dtor ] Pawn (derived) destructor running"); }
Piece* Pawn::clone() const { return new Pawn(*this); }
std::string Pawn::getName() const { return "Pawn"; }
int Pawn::getValue() const { return 1; }

// ===== [Rahmah] Polymorphic move validation (OVERRIDE) ======================

// A pawn attacks only the two squares diagonally in front of it.
bool Pawn::attacksSquare(const Board&, const Position& from, const Position& to) const {
    if (!to.isValid()) return false;
    int dir = (getColor() == Color::White) ? 1 : -1;
    return std::abs(to.col - from.col) == 1 && (to.row - from.row) == dir;
}

bool Pawn::isValidMove(const Board& board, const Position& from, const Position& to) const {
    if (!to.isValid() || from == to) return false;
    int dir = (getColor() == Color::White) ? 1 : -1;
    int startRow = (getColor() == Color::White) ? 1 : 6;
    int dr = to.row - from.row;
    int dc = to.col - from.col;

    if (dc == 0) {                                           // straight ahead (never captures)
        if (dr == dir) return board.isEmpty(to);
        if (dr == 2 * dir && from.row == startRow)
            return board.isEmpty(Position(from.row + dir, from.col)) && board.isEmpty(to);
        return false;
    }

    if (std::abs(dc) == 1 && dr == dir) {                    // diagonal: capture
        Piece* target = board.at(to);
        if (target) return target->getColor() != getColor();
        // empty square: only legal as an en-passant capture
        if (board.enPassantTarget().isValid() && to == board.enPassantTarget()) {
            Piece* victim = board.at(Position(from.row, to.col));
            return victim && victim->getType() == PieceType::Pawn &&
                   victim->getColor() != getColor();
        }
    }
    return false;
}
