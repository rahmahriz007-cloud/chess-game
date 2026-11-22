// ============================================================================
//  Demo.cpp  -  "./chess --demo"
//  A short guided tour that SHOWS each OOP concept running live.
// ============================================================================
#include "Demo.h"
#include "Board.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"
#include "PieceFactory.h"
#include <iostream>

namespace {

void heading(const std::string& text) {
    std::cout << "\n------------------------------------------------------------\n"
              << "  " << text << "\n"
              << "------------------------------------------------------------\n";
}

int countMoves(const Piece* piece, const Board& board, const Position& from) {
    int n = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (piece->isValidMove(board, from, Position(r, c))) ++n;
    return n;
}

} // namespace

void runDemo() {
    std::cout << "\n  C++ CHESS - OOP FEATURE DEMO\n";

    // ---- 1. Constructors & destructors (inheritance order) -----------------
    heading("1. CONSTRUCTORS and DESTRUCTORS  (base first, derived second)");
    Piece::setTrace(true);
    std::cout << "  Creating a Rook through a base-class pointer:  Piece* p = new Rook(White);\n";
    Piece* p = new Rook(Color::White);
    std::cout << "\n  Cloning it (copy constructor + virtual clone()):\n";
    Piece* copy = p->clone();
    std::cout << "\n  delete p;   <- the destructor is VIRTUAL, so the Rook part is cleaned up too:\n";
    delete p;
    std::cout << "\n  delete copy;\n";
    delete copy;
    Piece::setTrace(false);
    std::cout << "\n  Live Piece objects now: " << Piece::getLiveCount() << "  (no memory leak)\n";

    // ---- 2. Polymorphism & overriding ------------------------------------
    heading("2. POLYMORPHISM and OVERRIDING  (same call, different behaviour)");
    std::cout << "  One White piece on an otherwise EMPTY board, standing on d4.\n"
              << "  We call the SAME function  piece->isValidMove(...)  for every square:\n\n";
    const PieceType types[6] = {PieceType::King, PieceType::Queen, PieceType::Rook,
                                PieceType::Bishop, PieceType::Knight, PieceType::Pawn};
    for (PieceType t : types) {
        Board board;
        Position d4(3, 3);
        board.placePiece(createPiece(t, Color::White), d4);
        Piece* piece = board.at(d4);                      // only a Piece* is used here
        std::cout << "    " << piece->getName() << std::string(8 - piece->getName().size(), ' ')
                  << " value " << piece->getValue() << "   can reach "
                  << countMoves(piece, board, d4) << " squares\n";
    }

    // ---- 3. Abstraction -------------------------------------------------------------
    heading("3. ABSTRACTION  (Piece is an abstract class)");
    std::cout << "  'Piece* x = new Piece(...)' does not compile: Piece has pure virtual\n"
              << "  functions, so only King, Queen, Rook, Bishop, Knight and Pawn can exist.\n";

    // ---- 4. Rule of three (deep copy) --------------------------------------------------
    heading("4. RULE OF THREE  (Board owns its pieces: deep copy + destructor)");
    int before = Piece::getLiveCount();
    {
        Board a;
        a.setupInitialPosition();
        std::cout << "  Board a created:            live pieces = " << Piece::getLiveCount() - before << "\n";
        Board b = a;                                   // copy constructor
        std::cout << "  Board b = a  (deep copy):   live pieces = " << Piece::getLiveCount() - before << "\n";
        b.makeMove(Move(Position(1, 4), Position(3, 4)));   // e2-e4 on the COPY only
        std::cout << "  After e2-e4 on b:  a has a pawn on e2? "
                  << (a.at(Position(1, 4)) ? "yes" : "no") << "   b has a pawn on e2? "
                  << (b.at(Position(1, 4)) ? "yes" : "no") << "\n";
    }
    std::cout << "  Both boards left scope:     live pieces = " << Piece::getLiveCount() - before
              << "  (destructors freed everything)\n";

    // ---- 5. Encapsulation / operator overloading -------------------------------------------
    heading("5. ENCAPSULATION and OPERATOR OVERLOADING");
    Position e4(3, 4);
    std::cout << "  Piece data (colour, type, moved) is private -> read with getters.\n"
              << "  Position overloads << and ==:   printing a square gives: " << e4
              << "   and  e4 == Position::fromString(\"e4\") is "
              << (e4 == Position::fromString("e4") ? "true" : "false") << "\n\n";
}
