// Knight.cpp
//   [Insia Fatima]   constructor, destructor, clone, name, value
//   [Rahmah Rizwan]  isValidMove() override (the "L" jump)
#include "Knight.h"
#include "Board.h"
#include <cstdlib>

// ===== [Insia] Constructor / destructor / attributes =========================
Knight::Knight(Color color) : Piece(color, PieceType::Knight) {
    Piece::traceMessage("[ctor ] Knight (derived) constructor finished");
}
Knight::~Knight() { Piece::traceMessage("[dtor ] Knight (derived) destructor running"); }
Piece* Knight::clone() const { return new Knight(*this); }
std::string Knight::getName() const { return "Knight"; }
int Knight::getValue() const { return 3; }

// ===== [Rahmah] Polymorphic move validation (OVERRIDE) ======================
bool Knight::isValidMove(const Board& board, const Position& from, const Position& to) const {
    if (!to.isValid()) return false;
    int dr = std::abs(to.row - from.row);
    int dc = std::abs(to.col - from.col);
    bool lShape = (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
    return lShape && canLandOn(board, to);        // knights jump: no path check
}
