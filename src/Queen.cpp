// Queen.cpp
//   [Insia Fatima]   constructor, destructor, clone, name, value
//   [Rahmah Rizwan]  isValidMove() override (rook + bishop movement)
#include "Queen.h"
#include "Board.h"
#include <cstdlib>

// ===== [Insia] Constructor / destructor / attributes =========================
Queen::Queen(Color color) : Piece(color, PieceType::Queen) {
    Piece::traceMessage("[ctor ] Queen (derived) constructor finished");
}
Queen::~Queen() { Piece::traceMessage("[dtor ] Queen (derived) destructor running"); }
Piece* Queen::clone() const { return new Queen(*this); }
std::string Queen::getName() const { return "Queen"; }
int Queen::getValue() const { return 9; }

// ===== [Rahmah] Polymorphic move validation (OVERRIDE) ======================
bool Queen::isValidMove(const Board& board, const Position& from, const Position& to) const {
    if (!to.isValid() || from == to) return false;
    int dr = std::abs(to.row - from.row);
    int dc = std::abs(to.col - from.col);
    bool straight = (dr == 0 || dc == 0);
    bool diagonal = (dr == dc);
    if (!straight && !diagonal) return false;
    return pathClear(board, from, to) && canLandOn(board, to);
}
