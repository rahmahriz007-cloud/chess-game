// ============================================================================
//  test_chess.cpp  -  Automatic tests (no external library needed)
//
//  Run with:  make test        (add  --deep  for a few extra perft levels)
//
//  "Perft" counts every legal move sequence to a fixed depth.  The numbers
//  below are the well-known reference values from the chess-programming
//  community, so matching them proves castling, en passant, promotion,
//  check and pins are all implemented correctly.
// ============================================================================
#include "Board.h"
#include "PieceFactory.h"
#include <cstring>
#include <iostream>
#include <string>

static int g_checks = 0, g_failed = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        ++g_checks;                                                              \
        if (!(cond)) {                                                           \
            ++g_failed;                                                          \
            std::cout << "    FAILED  line " << __LINE__ << ":  " #cond "\n";    \
        }                                                                        \
    } while (0)

static bool loaded(Board& b, const std::string& fen, Color& side) {
    return b.loadFEN(fen, side);
}

static bool hasMove(const Board& b, Color side, const std::string& from, const std::string& to) {
    return b.isLegalMove(Position::fromString(from), Position::fromString(to), side);
}

static unsigned long long perft(const Board& b, Color side, int depth) {
    if (depth == 0) return 1;
    std::vector<Move> moves = b.legalMoves(side);
    if (depth == 1) return moves.size();
    unsigned long long nodes = 0;
    for (const Move& m : moves) {
        Board next(b);                       // copy constructor
        next.makeMove(m);
        nodes += perft(next, opposite(side), depth - 1);
    }
    return nodes;
}

static void runPerft(const char* name, const char* fen, const unsigned long long* expected, int maxDepth) {
    Board b;
    Color side;
    CHECK(loaded(b, fen, side));
    std::cout << "  perft  " << name << "\n";
    for (int d = 1; d <= maxDepth; ++d) {
        unsigned long long got = perft(b, side, d);
        bool ok = (got == expected[d - 1]);
        std::cout << "     depth " << d << ":  " << got << (ok ? "   OK\n" : "   WRONG (expected " + std::to_string(expected[d - 1]) + ")\n");
        CHECK(ok);
    }
}

int main(int argc, char* argv[]) {
    bool deep = (argc > 1 && std::strcmp(argv[1], "--deep") == 0);
    Color side = Color::White;

    std::cout << "\n== 1. Constructors / destructors / Rule of Three ==\n";
    {
        int before = Piece::getLiveCount();
        {
            Board a;
            a.setupInitialPosition();
            CHECK(Piece::getLiveCount() == before + 32);
            Board b(a);                                   // copy constructor
            CHECK(Piece::getLiveCount() == before + 64);
            Board c;
            c = a;                                        // copy assignment
            CHECK(Piece::getLiveCount() == before + 96);
            Board& alias = c;
            c = alias;                                    // self-assignment is safe
            CHECK(Piece::getLiveCount() == before + 96);
            b.makeMove(Move(Position::fromString("e2"), Position::fromString("e4")));
            CHECK(a.at(Position::fromString("e2")) != nullptr);   // copy is independent
        }
        CHECK(Piece::getLiveCount() == before);           // nothing leaked
    }

    std::cout << "== 2. Polymorphism: each piece overrides isValidMove ==\n";
    {
        Board b;
        b.setupInitialPosition();
        CHECK(hasMove(b, Color::White, "e2", "e4"));      // pawn double step
        CHECK(!hasMove(b, Color::White, "e2", "e5"));
        CHECK(hasMove(b, Color::White, "g1", "f3"));      // knight jump
        CHECK(!hasMove(b, Color::White, "g1", "g3"));
        CHECK(!hasMove(b, Color::White, "f1", "c4"));     // bishop blocked by own pawn
        CHECK(!hasMove(b, Color::White, "a1", "a3"));     // rook blocked
        CHECK(!hasMove(b, Color::White, "d1", "d3"));     // queen blocked
        CHECK(!hasMove(b, Color::White, "e1", "e2"));     // king blocked
        CHECK(!hasMove(b, Color::Black, "e2", "e4"));     // not your piece
        CHECK(b.legalMoves(Color::White).size() == 20);
    }

    std::cout << "== 3. Checkmate, stalemate, check ==\n";
    {
        Board b;
        b.setupInitialPosition();
        const char* foolsMate[4][2] = {{"f2", "f3"}, {"e7", "e5"}, {"g2", "g4"}, {"d8", "h4"}};
        Color turn = Color::White;
        for (auto& mv : foolsMate) {
            CHECK(hasMove(b, turn, mv[0], mv[1]));
            b.makeMove(Move(Position::fromString(mv[0]), Position::fromString(mv[1])));
            turn = opposite(turn);
        }
        CHECK(b.isInCheck(Color::White));
        CHECK(!b.hasAnyLegalMove(Color::White));          // fool's mate = checkmate

        Board s;
        CHECK(loaded(s, "7k/5Q2/6K1/8/8/8/8/8 b - - 0 1", side));
        CHECK(!s.isInCheck(Color::Black));
        CHECK(!s.hasAnyLegalMove(Color::Black));          // stalemate

        Board pin;                                         // a pinned piece may not move
        CHECK(loaded(pin, "4k3/8/8/8/8/4r3/4N3/4K3 w - - 0 1", side));
        CHECK(!hasMove(pin, Color::White, "e2", "c3"));
        CHECK(hasMove(pin, Color::White, "e1", "d1"));     // the king itself can still step aside
    }

    std::cout << "== 4. Castling ==\n";
    {
        Board b;
        CHECK(loaded(b, "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", side));
        CHECK(hasMove(b, Color::White, "e1", "g1"));
        CHECK(hasMove(b, Color::White, "e1", "c1"));
        b.makeMove(Move(Position::fromString("e1"), Position::fromString("g1")));
        CHECK(b.at(Position::fromString("g1")) && b.at(Position::fromString("g1"))->getType() == PieceType::King);
        CHECK(b.at(Position::fromString("f1")) && b.at(Position::fromString("f1"))->getType() == PieceType::Rook);
        CHECK(b.isEmpty(Position::fromString("h1")));

        Board t;   // black rook on f2 attacks f1: cannot castle king side through check
        CHECK(loaded(t, "r3k2r/8/8/8/8/8/5r2/R3K2R w KQkq - 0 1", side));
        CHECK(!hasMove(t, Color::White, "e1", "g1"));
        CHECK(hasMove(t, Color::White, "e1", "c1"));

        Board m;   // castling right gone after the king has moved
        CHECK(loaded(m, "r3k2r/8/8/8/8/8/8/R3K2R w kq - 0 1", side));
        CHECK(!hasMove(m, Color::White, "e1", "g1"));
        CHECK(hasMove(m, Color::Black, "e8", "g8"));
    }

    std::cout << "== 5. En passant ==\n";
    {
        Board b;
        CHECK(loaded(b, "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", side));
        CHECK(hasMove(b, Color::White, "e5", "d6"));
        MoveInfo info = b.makeMove(Move(Position::fromString("e5"), Position::fromString("d6")));
        CHECK(info.enPassant && info.capture);
        CHECK(b.isEmpty(Position::fromString("d5")));      // the captured pawn is gone
        CHECK(b.at(Position::fromString("d6")) != nullptr);

        Board c;                                            // ep only right after the double step
        c.setupInitialPosition();
        c.makeMove(Move(Position::fromString("e2"), Position::fromString("e4")));
        CHECK(c.enPassantTarget() == Position::fromString("e3"));
        c.makeMove(Move(Position::fromString("a7"), Position::fromString("a6")));
        CHECK(!c.enPassantTarget().isValid());
    }

    std::cout << "== 6. Promotion ==\n";
    {
        Board b;
        CHECK(loaded(b, "8/P6k/8/8/8/8/8/K7 w - - 0 1", side));
        int promos = 0;
        for (const Move& m : b.legalMoves(Color::White))
            if (m.from == Position::fromString("a7")) ++promos;
        CHECK(promos == 4);                                 // Q R B N
        MoveInfo info = b.makeMove(Move(Position::fromString("a7"), Position::fromString("a8"), 'N'));
        CHECK(info.promotion);
        CHECK(b.at(Position::fromString("a8"))->getType() == PieceType::Knight);
    }

    std::cout << "== 7. Draw by insufficient material ==\n";
    {
        Board a, b, c, d;
        CHECK(loaded(a, "8/8/8/4k3/8/8/3K4/8 w - - 0 1", side));
        CHECK(a.insufficientMaterial());                    // K v K
        CHECK(loaded(b, "8/8/8/4k3/8/8/3KB3/8 w - - 0 1", side));
        CHECK(b.insufficientMaterial());                    // K+B v K
        CHECK(loaded(c, "8/8/8/4k3/8/8/3KR3/8 w - - 0 1", side));
        CHECK(!c.insufficientMaterial());                   // K+R v K can mate
        CHECK(loaded(d, "8/8/8/4k3/8/8/3KP3/8 w - - 0 1", side));
        CHECK(!d.insufficientMaterial());
    }

    std::cout << "== 8. Invalid FEN is rejected ==\n";
    {
        Board b;
        int before = Piece::getLiveCount();
        CHECK(!b.loadFEN("not a fen", side));
        CHECK(!b.loadFEN("8/8/8/8/8/8/8/8 w - - 0 1", side));        // no kings
        CHECK(!b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w", side)); // 7 ranks
        CHECK(Piece::getLiveCount() == before);                         // failed loads leak nothing
    }

    std::cout << "== 9. Perft: counting every legal move sequence ==\n";
    {
        const unsigned long long start[] = {20, 400, 8902, 197281, 4865609};
        runPerft("Starting position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", start, deep ? 5 : 4);
        const unsigned long long kiwi[] = {48, 2039, 97862, 4085603};
        runPerft("Kiwipete (castling, pins, ep)", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", kiwi, deep ? 4 : 3);
        const unsigned long long p3[] = {14, 191, 2812, 43238, 674624};
        runPerft("Endgame with en-passant pins", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", p3, deep ? 5 : 4);
        const unsigned long long p4[] = {6, 264, 9467, 422333};
        runPerft("Promotions + castling", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", p4, deep ? 4 : 3);
        const unsigned long long p5[] = {44, 1486, 62379, 2103487};
        runPerft("Promotion with check", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", p5, deep ? 4 : 3);
    }

    std::cout << "\n  " << g_checks << " checks run, " << g_failed << " failed.\n";
    std::cout << (g_failed == 0 ? "  ALL TESTS PASSED\n\n" : "  SOME TESTS FAILED\n\n");
    return g_failed == 0 ? 0 : 1;
}
