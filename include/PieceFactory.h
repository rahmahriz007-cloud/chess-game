// ============================================================================
//  PieceFactory.h  -  Creates the right derived object and returns it as a
//                     Piece*  (this is polymorphism: callers only see Piece).
// ============================================================================
#ifndef PIECEFACTORY_H
#define PIECEFACTORY_H

#include "Piece.h"

Piece* createPiece(PieceType type, Color color);
Piece* createPieceFromSymbol(char symbol);                 // FEN letter -> piece, or nullptr
bool promotionTypeFromChar(char c, PieceType& typeOut);    // 'Q','R','B','N' (any case)

#endif
