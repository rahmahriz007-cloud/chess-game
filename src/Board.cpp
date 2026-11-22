// ============================================================================
//  Board.cpp  -  Board management
//
//  [Zainab Azram]  Rule of Three, set-up, FEN loading, placing / moving /
//                  capturing pieces, castling + en-passant + promotion
//                  execution, display, repetition keys, draw material test.
// ============================================================================
#include "Board.h"
#include "PieceFactory.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>

// ===== [Zainab] Constructors / destructor (Rule of Three) =====================

Board::Board() : epTarget_() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) grid_[r][c] = nullptr;
}

Board::Board(const Board& other) : epTarget_(other.epTarget_) {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) grid_[r][c] = nullptr;
    copyFrom(other);
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {            // protect against self-assignment
        clear();
        epTarget_ = other.epTarget_;
        copyFrom(other);
    }
    return *this;
}

Board::~Board() { clear(); }

void Board::clear() {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            delete grid_[r][c];      // virtual destructor -> correct derived dtor runs
            grid_[r][c] = nullptr;
        }
}

void Board::copyFrom(const Board& other) {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (other.grid_[r][c]) grid_[r][c] = other.grid_[r][c]->clone();   // polymorphic copy
}

// ===== [Zainab] Set-up ========================================================

void Board::setupInitialPosition() {
    clear();
    epTarget_ = Position();
    const PieceType backRank[8] = {PieceType::Rook, PieceType::Knight, PieceType::Bishop,
                                   PieceType::Queen, PieceType::King, PieceType::Bishop,
                                   PieceType::Knight, PieceType::Rook};
    for (int c = 0; c < 8; ++c) {
        grid_[0][c] = createPiece(backRank[c], Color::White);
        grid_[1][c] = createPiece(PieceType::Pawn, Color::White);
        grid_[6][c] = createPiece(PieceType::Pawn, Color::Black);
        grid_[7][c] = createPiece(backRank[c], Color::Black);
    }
}

void Board::placePiece(Piece* piece, const Position& pos) {
    if (!pos.isValid()) { delete piece; return; }
    delete grid_[pos.row][pos.col];
    grid_[pos.row][pos.col] = piece;
}

// Forsyth-Edwards Notation, e.g. "8/8/8/8/8/8/8/K6k w - - 0 1"
bool Board::loadFEN(const std::string& fen, Color& sideToMove, int* halfmoveClock, int* fullmoveNumber) {
    std::istringstream in(fen);
    std::string placement, sideStr, castling = "-", ep = "-", halfStr, fullStr;
    if (!(in >> placement >> sideStr)) return false;
    in >> castling >> ep >> halfStr >> fullStr;

    int half = 0, full = 1;
    try {
        if (!halfStr.empty()) half = std::stoi(halfStr);
        if (!fullStr.empty()) full = std::stoi(fullStr);
    } catch (...) { return false; }

    Board temp;                       // build on a scratch board so we never half-load
    int r = 7, c = 0, kings[2] = {0, 0};
    for (char ch : placement) {
        if (ch == '/') {
            if (c != 8 || r == 0) return false;
            --r; c = 0;
        } else if (ch >= '1' && ch <= '8') {
            c += ch - '0';
            if (c > 8) return false;
        } else {
            if (c >= 8) return false;
            Piece* p = createPieceFromSymbol(ch);
            if (!p) return false;
            if (p->getType() == PieceType::King) ++kings[p->getColor() == Color::White ? 0 : 1];
            temp.placePiece(p, Position(r, c));
            ++c;
        }
    }
    if (r != 0 || c != 8 || kings[0] != 1 || kings[1] != 1) return false;

    Color side;
    if (sideStr == "w") side = Color::White;
    else if (sideStr == "b") side = Color::Black;
    else return false;

    // Castling rights -> "has moved" flags of kings and rooks
    auto has = [&](char ch) { return castling.find(ch) != std::string::npos; };
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col) {
            Piece* p = temp.grid_[row][col];
            if (!p) continue;
            bool white = (p->getColor() == Color::White);
            int home = white ? 0 : 7;
            if (p->getType() == PieceType::King) {
                bool anyRight = white ? (has('K') || has('Q')) : (has('k') || has('q'));
                p->setMoved(!(row == home && col == 4 && anyRight));
            } else if (p->getType() == PieceType::Rook) {
                bool right = false;
                if (row == home && col == 7) right = white ? has('K') : has('k');
                if (row == home && col == 0) right = white ? has('Q') : has('q');
                p->setMoved(!right);
            }
        }

    if (ep != "-") {
        Position e = Position::fromString(ep);
        if (!e.isValid()) return false;
        temp.epTarget_ = e;
    }

    *this = temp;                     // copy-assignment (deep copy)
    sideToMove = side;
    if (halfmoveClock) *halfmoveClock = half;
    if (fullmoveNumber) *fullmoveNumber = full;
    return true;
}

// ===== [Zainab] Access ==========================================================

Piece* Board::at(const Position& pos) const {
    if (!pos.isValid()) return nullptr;
    return grid_[pos.row][pos.col];
}

bool Board::isEmpty(const Position& pos) const { return at(pos) == nullptr; }

Position Board::findKing(Color color) const {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* p = grid_[r][c];
            if (p && p->getType() == PieceType::King && p->getColor() == color) return Position(r, c);
        }
    return Position();
}

// ===== [Zainab] Playing a move: capture, castling, en passant, promotion ========

MoveInfo Board::makeMove(const Move& move) {
    MoveInfo info;
    Piece* piece = at(move.from);
    if (!piece || !move.to.isValid()) return info;

    info.valid = true;
    info.movedType = piece->getType();
    Color color = piece->getColor();
    Position newEnPassant;                       // invalid unless a pawn double-steps

    // 1) ordinary capture
    Piece* target = grid_[move.to.row][move.to.col];
    if (target) {
        info.capture = true;
        info.capturedType = target->getType();
    }

    // 2) en-passant capture: pawn moves diagonally onto the empty ep square
    if (piece->getType() == PieceType::Pawn && !target && move.from.col != move.to.col) {
        Position victim(move.from.row, move.to.col);
        if (at(victim)) {
            info.enPassant = true;
            info.capture = true;
            info.capturedType = PieceType::Pawn;
            delete grid_[victim.row][victim.col];
            grid_[victim.row][victim.col] = nullptr;
        }
    }

    // 3) castling: the king moves two squares, so the rook jumps over it
    if (piece->getType() == PieceType::King && std::abs(move.to.col - move.from.col) == 2) {
        bool kingSide = move.to.col > move.from.col;
        int rookFrom = kingSide ? 7 : 0;
        int rookTo = kingSide ? 5 : 3;
        Piece* rook = grid_[move.from.row][rookFrom];
        grid_[move.from.row][rookTo] = rook;
        grid_[move.from.row][rookFrom] = nullptr;
        if (rook) rook->setMoved(true);
        info.castleKingSide = kingSide;
        info.castleQueenSide = !kingSide;
    }

    // 4) move the piece (this deletes a captured piece standing on 'to')
    delete grid_[move.to.row][move.to.col];
    grid_[move.to.row][move.to.col] = piece;
    grid_[move.from.row][move.from.col] = nullptr;
    piece->setMoved(true);

    // 5) a pawn double step creates an en-passant target square
    if (piece->getType() == PieceType::Pawn && std::abs(move.to.row - move.from.row) == 2)
        newEnPassant = Position((move.from.row + move.to.row) / 2, move.from.col);

    // 6) promotion: replace the pawn by the chosen piece (default Queen)
    int lastRank = (color == Color::White) ? 7 : 0;
    if (piece->getType() == PieceType::Pawn && move.to.row == lastRank) {
        PieceType chosen = PieceType::Queen;
        promotionTypeFromChar(move.promotion, chosen);   // keeps Queen if invalid / 0
        Piece* promoted = createPiece(chosen, color);
        promoted->setMoved(true);
        delete grid_[move.to.row][move.to.col];          // delete the old pawn
        grid_[move.to.row][move.to.col] = promoted;
        info.promotion = true;
    }

    epTarget_ = newEnPassant;
    return info;
}

// ===== [Zainab] Display ==========================================================

void Board::display(bool unicode, bool useColor, const std::vector<Position>& highlights) const {
    std::cout << "\n      a  b  c  d  e  f  g  h\n";
    for (int r = 7; r >= 0; --r) {
        std::cout << "   " << (r + 1) << " ";
        for (int c = 0; c < 8; ++c) {
            Piece* p = grid_[r][c];
            bool hl = std::find(highlights.begin(), highlights.end(), Position(r, c)) != highlights.end();
            std::string glyph;
            if (p) {
                if (unicode) glyph = p->getUnicode(useColor);
                else glyph = std::string(1, useColor ? static_cast<char>(std::toupper(p->getSymbol()))
                                                     : p->getSymbol());
            }
            if (useColor) {
                const char* bg = hl ? (p ? "\033[48;5;160m" : "\033[48;5;70m")
                                    : (((r + c) % 2 == 0) ? "\033[48;5;94m" : "\033[48;5;180m");
                const char* fg = p ? (p->getColor() == Color::White ? "\033[1;38;5;231m"
                                                                    : "\033[1;38;5;16m") : "";
                std::cout << bg << fg << " " << (p ? glyph : std::string(" ")) << " " << "\033[0m";
            } else if (p) {
                std::cout << (hl ? "[" : " ") << glyph << (hl ? "]" : " ");
            } else {
                std::cout << (hl ? " * " : " . ");
            }
        }
        std::cout << "  " << (r + 1) << "\n";
    }
    std::cout << "      a  b  c  d  e  f  g  h\n\n";
}

// ===== [Zainab] Draw detection helpers ==============================================

bool Board::isUnmoved(PieceType type, Color color, const Position& pos) const {
    Piece* p = at(pos);
    return p && p->getType() == type && p->getColor() == color && !p->hasMoved();
}

std::string Board::castlingRights() const {
    std::string s;
    if (isUnmoved(PieceType::King, Color::White, Position(0, 4))) {
        if (isUnmoved(PieceType::Rook, Color::White, Position(0, 7))) s += 'K';
        if (isUnmoved(PieceType::Rook, Color::White, Position(0, 0))) s += 'Q';
    }
    if (isUnmoved(PieceType::King, Color::Black, Position(7, 4))) {
        if (isUnmoved(PieceType::Rook, Color::Black, Position(7, 7))) s += 'k';
        if (isUnmoved(PieceType::Rook, Color::Black, Position(7, 0))) s += 'q';
    }
    return s.empty() ? "-" : s;
}

// Same key == same position (pieces, side to move, castling, en passant)
std::string Board::positionKey(Color sideToMove) const {
    std::string key;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) key += grid_[r][c] ? grid_[r][c]->getSymbol() : '.';
    key += (sideToMove == Color::White) ? " w " : " b ";
    key += castlingRights();
    key += ' ';
    key += epTarget_.isValid() ? epTarget_.toString() : "-";
    return key;
}

// King vs King, King+minor vs King, or bishops all on the same colour: nobody can mate.
bool Board::insufficientMaterial() const {
    int knights = 0, bishops = 0;
    int bishopShade[2] = {0, 0};
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            Piece* p = grid_[r][c];
            if (!p) continue;
            switch (p->getType()) {
                case PieceType::Pawn:
                case PieceType::Rook:
                case PieceType::Queen:  return false;
                case PieceType::Knight: ++knights; break;
                case PieceType::Bishop: ++bishops; ++bishopShade[(r + c) % 2]; break;
                default: break;
            }
        }
    if (knights + bishops <= 1) return true;
    if (knights == 0 && (bishopShade[0] == 0 || bishopShade[1] == 0)) return true;
    return false;
}
