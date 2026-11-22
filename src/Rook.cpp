// Rook.cpp
//   [Insia Fatima]   constructor, destructor, clone, name, value
//   [Rahmah Rizwan]  isValidMove() override (straight lines)
#include "Rook.h"
#include "Board.h"
#include <cstdlib>

// ===== [Insia] Constructor / destructor / attributes =========================
Rook::Rook(Color color) : Piece(color, PieceType::Rook) {
    Piece::traceMessage("[ctor ] Rook (derived) constructor finished");
}
Rook::~Rook() { Piece::traceMessage("[dtor ] Rook (derived) destructor running"); }
Piece* Rook::clone() const { return new Rook(*this); }
std::string Rook::getName() const { return "Rook"; }
int Rook::getValue() const { return 5; }

// ===== [Rahmah] Polymorphic move validation (OVERRIDE) ======================
bool Rook::isValidMove(const Board& board, const Position& from, const Position& to) const {
    if (!to.isValid() || from == to) return false;
    if (from.row != to.row && from.col != to.col) return false;   // not a straight line
    return pathClear(board, from, to) && canLandOn(board, to);
}
