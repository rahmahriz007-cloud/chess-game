// ============================================================================
//  Knight.h  -  Derived class: inherits from Piece and OVERRIDES its virtuals
// ============================================================================
#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"

class Knight : public Piece {
public:
    explicit Knight(Color color);   // constructor
    ~Knight() override;             // destructor

    bool isValidMove(const Board& board, const Position& from,
                     const Position& to) const override;

    Piece* clone() const override;
    std::string getName() const override;
    int getValue() const override;
};

#endif // KNIGHT_H
