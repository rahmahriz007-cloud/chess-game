// Bishop.cpp
//   [Insia Fatima]   constructor, destructor, clone, name, value
//   [Rahmah Rizwan]  isValidMove() override (diagonals)
#include "Bishop.h"
#include "Board.h"
#include <cstdlib>

// ===== [Insia] Constructor / destructor / attributes =========================
Bishop::Bishop(Color color) : Piece(color, PieceType::Bishop) {
    Piece::traceMessage("[ctor ] Bishop (derived) constructor finished");
}
Bishop::~Bishop() { Piece::traceMessage("[dtor ] Bishop (derived) destructor running"); }
Piece* Bishop::clone() const { return new Bishop(*this); }
std::string Bishop::getName() const { return "Bishop"; }
int Bishop::getValue() const { return 3; }

// ===== [Rahmah] Polymorphic move validation (OVERRIDE) ======================
bool Bishop::isValidMove(const Board& board, const Position& from, const Position& to) const {
    if (!to.isValid() || from == to) return false;
    if (std::abs(to.row - from.row) != std::abs(to.col - from.col)) return false;
    return pathClear(board, from, to) && canLandOn(board, to);
}
