#include "PieceFactory.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"
#include <cctype>

Piece* createPiece(PieceType type, Color color) {
    switch (type) {
        case PieceType::King:   return new King(color);
        case PieceType::Queen:  return new Queen(color);
        case PieceType::Rook:   return new Rook(color);
        case PieceType::Bishop: return new Bishop(color);
        case PieceType::Knight: return new Knight(color);
        case PieceType::Pawn:   return new Pawn(color);
    }
    return nullptr;
}

Piece* createPieceFromSymbol(char symbol) {
    Color color = std::isupper(static_cast<unsigned char>(symbol)) ? Color::White : Color::Black;
    switch (std::toupper(static_cast<unsigned char>(symbol))) {
        case 'K': return new King(color);
        case 'Q': return new Queen(color);
        case 'R': return new Rook(color);
        case 'B': return new Bishop(color);
        case 'N': return new Knight(color);
        case 'P': return new Pawn(color);
        default:  return nullptr;
    }
}

bool promotionTypeFromChar(char c, PieceType& typeOut) {
    switch (std::toupper(static_cast<unsigned char>(c))) {
        case 'Q': typeOut = PieceType::Queen;  return true;
        case 'R': typeOut = PieceType::Rook;   return true;
        case 'B': typeOut = PieceType::Bishop; return true;
        case 'N': typeOut = PieceType::Knight; return true;
        default:  return false;
    }
}
