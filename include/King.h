// ============================================================================
//  King.h  -  Derived class: inherits from Piece and OVERRIDES its virtuals
// ============================================================================
#ifndef KING_H
#define KING_H

#include "Piece.h"

class King : public Piece {
public:
    explicit King(Color color);   // constructor
    ~King() override;             // destructor

    bool isValidMove(const Board& board, const Position& from,
                     const Position& to) const override;
    bool attacksSquare(const Board& board, const Position& from,
                       const Position& to) const override;

    Piece* clone() const override;
    std::string getName() const override;
    int getValue() const override;

private:
    bool canCastle(const Board& board, const Position& from,
                   const Position& to) const;   // castling rules
};

#endif // KING_H
