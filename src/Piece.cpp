// ============================================================================
//  Piece.cpp  -  Base class implementation
//
//  [Insia Fatima]  Constructors, copy constructor, virtual destructor,
//                  encapsulated attributes, symbols.
//  [Rahmah Rizwan] Shared movement helpers: pathClear(), canLandOn(),
//                  default attacksSquare().
// ============================================================================
#include "Piece.h"
#include "Board.h"
#include <cctype>
#include <iostream>

int Piece::liveCount_ = 0;
bool Piece::trace_ = false;

// ===== [Insia] Constructors / destructor =====================================

static const char* typeLabel(PieceType t) {
    switch (t) {
        case PieceType::King:   return "King";
        case PieceType::Queen:  return "Queen";
        case PieceType::Rook:   return "Rook";
        case PieceType::Bishop: return "Bishop";
        case PieceType::Knight: return "Knight";
        default:                return "Pawn";
    }
}

Piece::Piece(Color color, PieceType type)
    : color_(color), type_(type), moved_(false) {
    ++liveCount_;
    if (trace_)
        std::cout << "   [ctor ] Piece (base) built: " << colorName(color_) << " "
                  << typeLabel(type_) << "   (live pieces: " << liveCount_ << ")\n";
}

Piece::Piece(const Piece& other)
    : color_(other.color_), type_(other.type_), moved_(other.moved_) {
    ++liveCount_;
    if (trace_)
        std::cout << "   [copy ] Piece (base) copied: " << colorName(color_) << " "
                  << typeLabel(type_) << "   (live pieces: " << liveCount_ << ")\n";
}

Piece::~Piece() {
    --liveCount_;
    if (trace_)
        std::cout << "   [dtor ] Piece (base) destroyed: " << colorName(color_) << " "
                  << typeLabel(type_) << "   (live pieces: " << liveCount_ << ")\n";
}

// ===== [Insia] Encapsulated attributes / symbols ============================

char Piece::getSymbol() const {
    char c = 'P';
    switch (type_) {
        case PieceType::King:   c = 'K'; break;
        case PieceType::Queen:  c = 'Q'; break;
        case PieceType::Rook:   c = 'R'; break;
        case PieceType::Bishop: c = 'B'; break;
        case PieceType::Knight: c = 'N'; break;
        case PieceType::Pawn:   c = 'P'; break;
    }
    if (color_ == Color::Black) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return c;
}

std::string Piece::getUnicode(bool solid) const {
    // UTF-8 byte sequences for U+2654..U+265F (written as bytes so the file
    // compiles the same on every compiler).
    static const char* hollow[6] = {"\xE2\x99\x94", "\xE2\x99\x95", "\xE2\x99\x96",
                                    "\xE2\x99\x97", "\xE2\x99\x98", "\xE2\x99\x99"};
    static const char* filled[6] = {"\xE2\x99\x9A", "\xE2\x99\x9B", "\xE2\x99\x9C",
                                    "\xE2\x99\x9D", "\xE2\x99\x9E", "\xE2\x99\x9F"};
    int idx = static_cast<int>(type_);   // enum order matches the arrays
    bool useFilled = solid || color_ == Color::Black;
    return useFilled ? filled[idx] : hollow[idx];
}

// ===== [Insia] Life-cycle helpers =============================================

int Piece::getLiveCount() { return liveCount_; }
void Piece::setTrace(bool on) { trace_ = on; }
bool Piece::isTracing() { return trace_; }
void Piece::traceMessage(const std::string& text) {
    if (trace_) std::cout << "   " << text << "\n";
}

// ===== [Rahmah] Shared movement helpers ====================================

bool Piece::attacksSquare(const Board& board, const Position& from, const Position& to) const {
    // Default: a piece attacks exactly the squares it could move/capture to.
    return isValidMove(board, from, to);
}

// True when every square strictly BETWEEN 'from' and 'to' is empty.
// Only meaningful for straight or diagonal lines.
bool Piece::pathClear(const Board& board, const Position& from, const Position& to) const {
    int dr = (to.row > from.row) - (to.row < from.row);
    int dc = (to.col > from.col) - (to.col < from.col);
    Position cur(from.row + dr, from.col + dc);
    while (cur != to) {
        if (!board.isEmpty(cur)) return false;
        cur.row += dr;
        cur.col += dc;
    }
    return true;
}

// A piece may land on an empty square or on an enemy piece (a capture).
bool Piece::canLandOn(const Board& board, const Position& to) const {
    Piece* target = board.at(to);
    return target == nullptr || target->getColor() != color_;
}
