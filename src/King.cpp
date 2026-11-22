// King.cpp
//   [Insia Fatima]   constructor, destructor, clone, name, value
//   [Rahmah Rizwan]  isValidMove() / attacksSquare() overrides + castling rules
#include "King.h"
#include "Board.h"
#include <cstdlib>

// ===== [Insia] Constructor / destructor / attributes =========================
King::King(Color color) : Piece(color, PieceType::King) {
    Piece::traceMessage("[ctor ] King (derived) constructor finished");
}
King::~King() { Piece::traceMessage("[dtor ] King (derived) destructor running"); }
Piece* King::clone() const { return new King(*this); }
std::string King::getName() const { return "King"; }
int King::getValue() const { return 1000; }   // priceless - bigger than all material

// ===== [Rahmah] Polymorphic move validation (OVERRIDE) ======================

// The king ATTACKS the 8 neighbouring squares (castling never attacks).
bool King::attacksSquare(const Board&, const Position& from, const Position& to) const {
    if (!to.isValid() || from == to) return false;
    return std::abs(to.row - from.row) <= 1 && std::abs(to.col - from.col) <= 1;
}

bool King::isValidMove(const Board& board, const Position& from, const Position& to) const {
    if (attacksSquare(board, from, to) && canLandOn(board, to)) return true;   // normal step
    return canCastle(board, from, to);                                           // special move
}

// Castling: king and rook unmoved, squares between empty, king not in check,
// and the king may not pass through or land on an attacked square.
bool King::canCastle(const Board& board, const Position& from, const Position& to) const {
    if (hasMoved() || !to.isValid()) return false;
    int homeRow = (getColor() == Color::White) ? 0 : 7;
    if (from.row != homeRow || from.col != 4 || to.row != homeRow) return false;

    int dir;
    if (to.col == 6)      dir = 1;    // king side
    else if (to.col == 2) dir = -1;   // queen side
    else return false;

    int rookCol = (dir == 1) ? 7 : 0;
    Piece* rook = board.at(Position(homeRow, rookCol));
    if (!rook || rook->getType() != PieceType::Rook ||
        rook->getColor() != getColor() || rook->hasMoved())
        return false;

    for (int c = from.col + dir; c != rookCol; c += dir)          // squares in between
        if (!board.isEmpty(Position(homeRow, c))) return false;

    Color enemy = opposite(getColor());
    if (board.isSquareAttacked(from, enemy)) return false;        // cannot castle out of check
    for (int step = 1; step <= 2; ++step)                          // ...through check or into check
        if (board.isSquareAttacked(Position(homeRow, from.col + dir * step), enemy)) return false;
    return true;
}
