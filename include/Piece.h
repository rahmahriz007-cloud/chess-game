// ============================================================================
//  Piece.h  -  ABSTRACT base class of every chess piece
//
//  OOP concepts shown here
//    * Abstraction   : Piece cannot be created directly (pure virtual methods)
//    * Encapsulation : data members are private, accessed through getters
//    * Polymorphism  : isValidMove() / attacksSquare() are virtual and are
//                      OVERRIDDEN by King, Queen, Rook, Bishop, Knight, Pawn
//    * Constructors / destructors : base ctor, copy ctor, VIRTUAL destructor
// ============================================================================
#ifndef PIECE_H
#define PIECE_H

#include "Types.h"
#include <string>

class Board; // forward declaration (Board needs Piece, Piece needs Board)

class Piece {
public:
    // ----- Constructors / destructor ---------------------------------------
    Piece(Color color, PieceType type);   // normal constructor
    Piece(const Piece& other);            // copy constructor (used by clone())
    virtual ~Piece();                     // VIRTUAL so derived destructors run
    Piece& operator=(const Piece&) = delete;

    // ----- Polymorphic interface (overridden in every derived class) -------
    // Can this piece, standing on 'from', legally *move* to 'to' by its own
    // movement rules?  (King safety is checked later by the Board.)
    virtual bool isValidMove(const Board& board, const Position& from,
                             const Position& to) const = 0;

    // Does this piece *attack* square 'to'?  By default that is the same as a
    // valid move, but Pawn (diagonal only) and King (no castling) override it.
    virtual bool attacksSquare(const Board& board, const Position& from,
                               const Position& to) const;

    virtual Piece* clone() const = 0;            // "virtual copy constructor"
    virtual std::string getName() const = 0;     // "Rook", "Knight", ...
    virtual int getValue() const = 0;            // material value (1,3,3,5,9)

    // ----- Encapsulated attributes ------------------------------------------
    Color getColor() const { return color_; }
    PieceType getType() const { return type_; }
    bool hasMoved() const { return moved_; }
    void setMoved(bool moved) { moved_ = moved; }
    char getSymbol() const;                      // 'K' for white, 'k' for black
    std::string getUnicode(bool solid = false) const;

    // ----- Object life-cycle helpers (used by the demo and the tests) ------
    static int getLiveCount();                   // how many Piece objects exist
    static void setTrace(bool on);               // print ctor/dtor messages
    static bool isTracing();
    static void traceMessage(const std::string& text);

protected:
    // Helpers shared by the derived classes
    bool pathClear(const Board& board, const Position& from, const Position& to) const;
    bool canLandOn(const Board& board, const Position& to) const;

private:
    Color color_;
    PieceType type_;
    bool moved_;

    static int liveCount_;
    static bool trace_;
};

#endif // PIECE_H
