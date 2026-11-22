// ============================================================================
//  BoardRules.cpp  -  Move validation at board level (still class Board)
//
//  [Rahmah Rizwan]  Check detection, legal-move filtering (a move may never
//                   leave your own king in check), legal-move generation,
//                   checkmate / stalemate support.
// ============================================================================
#include "Board.h"

// Is 'square' attacked by any piece of colour 'byColor'?
// Each piece answers through the VIRTUAL attacksSquare() -> polymorphism.
bool Board::isSquareAttacked(const Position& square, Color byColor) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* p = grid_[r][c];
            if (p && p->getColor() == byColor && p->attacksSquare(*this, Position(r, c), square))
                return true;
        }
    return false;
}

bool Board::isInCheck(Color color) const {
    Position king = findKing(color);
    if (!king.isValid()) return false;
    return isSquareAttacked(king, opposite(color));
}

// Try the move on the board, test for check, then put everything back.
bool Board::leavesKingInCheck(const Position& from, const Position& to) const {
    Piece* mover = grid_[from.row][from.col];
    Color side = mover->getColor();
    Piece* captured = grid_[to.row][to.col];

    Position epPos;
    Piece* epVictim = nullptr;
    if (mover->getType() == PieceType::Pawn && from.col != to.col && !captured) {
        epPos = Position(from.row, to.col);               // en-passant removes a pawn beside us
        epVictim = grid_[epPos.row][epPos.col];
        grid_[epPos.row][epPos.col] = nullptr;
    }

    grid_[to.row][to.col] = mover;
    grid_[from.row][from.col] = nullptr;
    bool inCheck = isInCheck(side);
    grid_[from.row][from.col] = mover;                     // undo
    grid_[to.row][to.col] = captured;
    if (epVictim) grid_[epPos.row][epPos.col] = epVictim;
    return inCheck;
}

// A move is LEGAL when (1) the piece's own rules allow it and
// (2) it does not leave the mover's king in check.
bool Board::isLegalMove(const Position& from, const Position& to, Color side) const {
    if (!from.isValid() || !to.isValid()) return false;
    Piece* piece = grid_[from.row][from.col];
    if (!piece || piece->getColor() != side) return false;
    if (!piece->isValidMove(*this, from, to)) return false;   // polymorphic call
    return !leavesKingInCheck(from, to);
}

std::vector<Position> Board::legalMovesFrom(const Position& from) const {
    std::vector<Position> result;
    Piece* piece = at(from);
    if (!piece) return result;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (isLegalMove(from, Position(r, c), piece->getColor())) result.push_back(Position(r, c));
    return result;
}

std::vector<Move> Board::legalMoves(Color side) const {
    std::vector<Move> moves;
    const char promos[4] = {'Q', 'R', 'B', 'N'};
    int lastRank = (side == Color::White) ? 7 : 0;
    for (int fr = 0; fr < 8; ++fr)
        for (int fc = 0; fc < 8; ++fc) {
            Piece* p = grid_[fr][fc];
            if (!p || p->getColor() != side) continue;
            Position from(fr, fc);
            for (int tr = 0; tr < 8; ++tr)
                for (int tc = 0; tc < 8; ++tc) {
                    Position to(tr, tc);
                    if (!isLegalMove(from, to, side)) continue;
                    if (p->getType() == PieceType::Pawn && tr == lastRank)
                        for (char promo : promos) moves.push_back(Move(from, to, promo));
                    else
                        moves.push_back(Move(from, to));
                }
        }
    return moves;
}

// Checkmate = in check and no legal move.  Stalemate = NOT in check and no legal move.
bool Board::hasAnyLegalMove(Color side) const {
    for (int fr = 0; fr < 8; ++fr)
        for (int fc = 0; fc < 8; ++fc) {
            Piece* p = grid_[fr][fc];
            if (!p || p->getColor() != side) continue;
            for (int tr = 0; tr < 8; ++tr)
                for (int tc = 0; tc < 8; ++tc)
                    if (isLegalMove(Position(fr, fc), Position(tr, tc), side)) return true;
        }
    return false;
}
