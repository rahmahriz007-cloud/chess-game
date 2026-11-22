// ============================================================================
//  Pawn.h  -  Derived class: inherits from Piece and OVERRIDES its virtuals
// ============================================================================
#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

class Pawn : public Piece {
public:
    explicit Pawn(Color color);   // constructor
    ~Pawn() override;             // destructor

    bool isValidMove(const Board& board, const Position& from,
                     const Position& to) const override;
    bool attacksSquare(const Board& board, const Position& from,
                       const Position& to) const override;

    Piece* clone() const override;
    std::string getName() const override;
    int getValue() const override;
};

#endif // PAWN_H
