// ============================================================================
//  Rook.h  -  Derived class: inherits from Piece and OVERRIDES its virtuals
// ============================================================================
#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

class Rook : public Piece {
public:
    explicit Rook(Color color);   // constructor
    ~Rook() override;             // destructor

    bool isValidMove(const Board& board, const Position& from,
                     const Position& to) const override;

    Piece* clone() const override;
    std::string getName() const override;
    int getValue() const override;
};

#endif // ROOK_H
