// ============================================================================
//  Board.h  -  The 8x8 chess board
//
//  The Board OWNS the pieces it stores, so it follows the "Rule of Three":
//      destructor + copy constructor + copy assignment
//  (deep copies are made with Piece::clone(), a polymorphic copy).
// ============================================================================
#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include "Types.h"
#include <string>
#include <vector>

class Board {
public:
    // ----- Rule of Three ----------------------------------------------------
    Board();                              // creates an EMPTY board
    Board(const Board& other);            // deep copy
    Board& operator=(const Board& other); // deep copy assignment
    ~Board();                             // deletes every piece

    // ----- Setting up --------------------------------------------------------
    void setupInitialPosition();
    bool loadFEN(const std::string& fen, Color& sideToMove,
                 int* halfmoveClock = nullptr, int* fullmoveNumber = nullptr);
    void placePiece(Piece* piece, const Position& pos);   // board takes ownership

    // ----- Looking at the board ------------------------------------------------
    Piece* at(const Position& pos) const;
    bool isEmpty(const Position& pos) const;
    Position findKing(Color color) const;
    Position enPassantTarget() const { return epTarget_; }

    // ----- Rules / move validation (check, legality, move generation) -------
    bool isSquareAttacked(const Position& square, Color byColor) const;
    bool isInCheck(Color color) const;
    bool isLegalMove(const Position& from, const Position& to, Color side) const;
    std::vector<Position> legalMovesFrom(const Position& from) const;
    std::vector<Move> legalMoves(Color side) const;
    bool hasAnyLegalMove(Color side) const;
    bool insufficientMaterial() const;

    // ----- Playing moves -----------------------------------------------------------
    MoveInfo makeMove(const Move& move);   // assumes the move is legal

    // ----- Output ------------------------------------------------------------
    void display(bool unicode, bool useColor,
                 const std::vector<Position>& highlights = std::vector<Position>()) const;
    std::string positionKey(Color sideToMove) const;   // for repetition detection

private:
    // 'mutable' because look-ahead temporarily moves pointers around inside
    // const methods (everything is restored before the method returns).
    mutable Piece* grid_[8][8];
    Position epTarget_;                     // en-passant square (invalid if none)

    void clear();
    void copyFrom(const Board& other);
    bool leavesKingInCheck(const Position& from, const Position& to) const;
    bool isUnmoved(PieceType type, Color color, const Position& pos) const;
    std::string castlingRights() const;
};

#endif // BOARD_H
