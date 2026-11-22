// ============================================================================
//  Queen.h  -  Derived class: inherits from Piece and OVERRIDES its virtuals
// ============================================================================
#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

class Queen : public Piece {
public:
    explicit Queen(Color color);   // constructor
    ~Queen() override;             // destructor

    bool isValidMove(const Board& board, const Position& from,
                     const Position& to) const override;

    Piece* clone() const override;
    std::string getName() const override;
    int getValue() const override;
};

#endif // QUEEN_H
