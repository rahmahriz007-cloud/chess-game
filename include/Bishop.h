// ============================================================================
//  Bishop.h  -  Derived class: inherits from Piece and OVERRIDES its virtuals
// ============================================================================
#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

class Bishop : public Piece {
public:
    explicit Bishop(Color color);   // constructor
    ~Bishop() override;             // destructor

    bool isValidMove(const Board& board, const Position& from,
                     const Position& to) const override;

    Piece* clone() const override;
    std::string getName() const override;
    int getValue() const override;
};

#endif // BISHOP_H
