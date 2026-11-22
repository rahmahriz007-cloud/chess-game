// ============================================================================
//  Game.cpp  -  Game logic
//
//  [Zainab Azram]  Player turns, reading moves, updating piece positions,
//                  captures, undo, move history (algebraic notation) and the
//                  game-ending conditions (checkmate, stalemate, draws).
// ============================================================================
#include "Game.h"
#include "PieceFactory.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace {

std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::string toLower(std::string s) {
    for (char& ch : s) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return s;
}

char pieceLetter(PieceType t) {
    switch (t) {
        case PieceType::King:   return 'K';
        case PieceType::Queen:  return 'Q';
        case PieceType::Rook:   return 'R';
        case PieceType::Bishop: return 'B';
        case PieceType::Knight: return 'N';
        default:                return 'P';
    }
}

} // namespace

// ----------------------------------------------------------------------------
Game::Game(bool unicode, bool color)
    : board_(), turn_(Color::White), halfmoveClock_(0), fullmoveNumber_(1),
      startTurn_(Color::White), startFullmove_(1), unicode_(unicode), color_(color),
      gameOver_(false) {
    board_.setupInitialPosition();
    resetTracking();
}

void Game::resetTracking() {
    history_.clear();
    undoStack_.clear();
    repetitions_.clear();
    repetitions_[board_.positionKey(turn_)] = 1;
    startTurn_ = turn_;
    startFullmove_ = fullmoveNumber_;
    gameOver_ = false;
    result_.clear();
}

bool Game::loadFEN(const std::string& fen) {
    Color side;
    int half = 0, full = 1;
    if (!board_.loadFEN(fen, side, &half, &full)) return false;
    turn_ = side;
    halfmoveClock_ = half;
    fullmoveNumber_ = full;
    resetTracking();
    return true;
}

// ----------------------------------------------------------------------------
void Game::printBanner() const {
    std::cout << "\n"
              << "  ==========================================\n"
              << "        C + +   C H E S S   G A M E\n"
              << "      Object-Oriented Programming Project\n"
              << "  ==========================================\n"
              << "  Type a move like  e2 e4  (or e2e4).  Type 'help' for commands.\n";
}

void Game::printHelp() const {
    std::cout << "\n  HOW TO PLAY\n"
              << "    e2 e4        move the piece on e2 to e4  (also: e2e4, e2-e4)\n"
              << "    e7 e8 q      promotion: add q, r, b or n (or you will be asked)\n"
              << "    e1 g1        castle king side  (e1 c1 = queen side)\n\n"
              << "  COMMANDS\n"
              << "    moves e2     show every legal move of the piece on e2\n"
              << "    history      list the moves played so far\n"
              << "    undo         take back the last move\n"
              << "    draw         offer a draw\n"
              << "    resign       give up the game\n"
              << "    help         show this help\n"
              << "    quit         leave the game\n\n"
              << "  BOARD: capital letters = White, small letters = Black\n"
              << "         K king  Q queen  R rook  B bishop  N knight  P pawn\n";
}

void Game::printHistory() const {
    if (history_.empty()) { std::cout << "  No moves have been played yet.\n"; return; }
    std::cout << "  ";
    int number = startFullmove_;
    Color c = startTurn_;
    for (size_t i = 0; i < history_.size(); ++i) {
        if (c == Color::White) std::cout << number << ". ";
        else if (i == 0) std::cout << number << "... ";
        std::cout << history_[i] << " ";
        if (c == Color::Black) ++number;
        c = opposite(c);
    }
    std::cout << "\n";
}

void Game::printStatus() const {
    std::cout << "  Move " << fullmoveNumber_ << "  |  " << colorName(turn_) << " to move";
    if (!history_.empty()) std::cout << "  |  last: " << history_.back();
    if (board_.isInCheck(turn_)) std::cout << "   ** CHECK! **";
    std::cout << "\n";
}

// ----------------------------------------------------------------------------
void Game::saveSnapshot() {
    Snapshot s{board_, turn_, halfmoveClock_, fullmoveNumber_, history_, repetitions_};
    undoStack_.push_back(s);
}

bool Game::undo() {
    if (undoStack_.empty()) return false;
    Snapshot& s = undoStack_.back();
    board_ = s.board;                   // Board copy-assignment (deep copy)
    turn_ = s.turn;
    halfmoveClock_ = s.halfmoveClock;
    fullmoveNumber_ = s.fullmoveNumber;
    history_ = s.history;
    repetitions_ = s.repetitions;
    undoStack_.pop_back();
    return true;
}

// ----------------------------------------------------------------------------
bool Game::handleCommand(const std::string& line) {
    std::istringstream ss(line);
    std::string cmd, arg;
    ss >> cmd >> arg;

    if (cmd == "help" || cmd == "h" || cmd == "?") { printHelp(); return true; }

    if (cmd == "quit" || cmd == "exit" || cmd == "q") {
        gameOver_ = true;
        result_ = "Game ended by player. Thanks for playing!";
        return true;
    }
    if (cmd == "history") { printHistory(); return true; }

    if (cmd == "undo") {
        if (undo()) std::cout << "  Last move taken back.\n";
        else std::cout << "  Nothing to undo.\n";
        return true;
    }
    if (cmd == "resign") {
        gameOver_ = true;
        result_ = std::string(colorName(turn_)) + " resigns. " + colorName(opposite(turn_)) + " wins!";
        return true;
    }
    if (cmd == "draw") {
        std::cout << "  " << colorName(opposite(turn_)) << ", do you accept the draw? (y/n): " << std::flush;
        std::string answer;
        if (std::getline(std::cin, answer) && !trim(answer).empty() &&
            std::tolower(static_cast<unsigned char>(trim(answer)[0])) == 'y') {
            gameOver_ = true;
            result_ = "Draw by agreement.";
        } else {
            std::cout << "  Draw declined - play on.\n";
        }
        return true;
    }
    if (cmd == "moves") { showMoves(arg); return true; }
    return false;
}

void Game::showMoves(const std::string& square) const {
    Position from = Position::fromString(square);
    Piece* p = board_.at(from);
    if (!from.isValid() || !p) { std::cout << "  Usage: moves e2   (a square that holds a piece)\n"; return; }
    if (p->getColor() != turn_) { std::cout << "  That piece belongs to " << colorName(opposite(turn_)) << ".\n"; return; }
    std::vector<Position> targets = board_.legalMovesFrom(from);
    if (targets.empty()) { std::cout << "  The " << p->getName() << " on " << from << " has no legal moves.\n"; return; }
    std::cout << "  " << p->getName() << " on " << from << " can go to:";
    for (const Position& t : targets) std::cout << " " << t;
    std::cout << "\n";
    board_.display(unicode_, color_, targets);
}

char Game::askPromotion() const {
    std::string line;
    while (true) {
        std::cout << "  Promote pawn to (Q)ueen, (R)ook, (B)ishop or k(N)ight? " << std::flush;
        if (!std::getline(std::cin, line)) return 'Q';
        line = trim(line);
        PieceType t;
        if (!line.empty() && promotionTypeFromChar(line[0], t))
            return static_cast<char>(std::toupper(static_cast<unsigned char>(line[0])));
        std::cout << "  Please answer Q, R, B or N.\n";
    }
}

// Standard algebraic notation (SAN), e.g. Nf3, exd5, O-O, e8=Q
std::string Game::buildSAN(const Move& m) const {
    Piece* p = board_.at(m.from);
    if (!p) return "";
    int dc = m.to.col - m.from.col;
    if (p->getType() == PieceType::King && std::abs(dc) == 2) return dc > 0 ? "O-O" : "O-O-O";

    bool capture = !board_.isEmpty(m.to) ||
                   (p->getType() == PieceType::Pawn && m.from.col != m.to.col);
    std::string s;
    if (p->getType() == PieceType::Pawn) {
        if (capture) s += static_cast<char>('a' + m.from.col);
    } else {
        s += pieceLetter(p->getType());
        bool other = false, sameFile = false, sameRank = false;   // disambiguation
        for (int r = 0; r < 8; ++r)
            for (int c = 0; c < 8; ++c) {
                Position q(r, c);
                if (q == m.from) continue;
                Piece* o = board_.at(q);
                if (o && o->getType() == p->getType() && o->getColor() == p->getColor() &&
                    board_.isLegalMove(q, m.to, p->getColor())) {
                    other = true;
                    if (q.col == m.from.col) sameFile = true;
                    if (q.row == m.from.row) sameRank = true;
                }
            }
        if (other) {
            if (!sameFile) s += static_cast<char>('a' + m.from.col);
            else if (!sameRank) s += static_cast<char>('1' + m.from.row);
            else s += m.from.toString();
        }
    }
    if (capture) s += 'x';
    s += m.to.toString();
    if (p->getType() == PieceType::Pawn && m.to.row == (p->getColor() == Color::White ? 7 : 0)) {
        s += '=';
        s += static_cast<char>(std::toupper(static_cast<unsigned char>(m.promotion ? m.promotion : 'Q')));
    }
    return s;
}

// ----------------------------------------------------------------------------
void Game::tryMove(const std::string& line) {
    std::string s;
    for (char ch : line)
        if (!std::isspace(static_cast<unsigned char>(ch)) && ch != '-' && ch != 'x' && ch != '=') s += ch;

    if (s.size() != 4 && s.size() != 5) {
        std::cout << "  I did not understand that. Try a move like  e2 e4  or type 'help'.\n";
        return;
    }
    Position from = Position::fromString(s.substr(0, 2));
    Position to = Position::fromString(s.substr(2, 2));
    if (!from.isValid() || !to.isValid()) {
        std::cout << "  Squares go from a1 to h8. Try again, e.g.  e2 e4\n";
        return;
    }

    Piece* piece = board_.at(from);
    if (!piece) { std::cout << "  There is no piece on " << from << ".\n"; return; }
    if (piece->getColor() != turn_) {
        std::cout << "  That piece belongs to " << colorName(opposite(turn_)) << " - it is " << colorName(turn_) << "'s turn.\n";
        return;
    }
    if (!board_.isLegalMove(from, to, turn_)) {
        if (!piece->isValidMove(board_, from, to))
            std::cout << "  Illegal move: a " << piece->getName() << " cannot go from " << from << " to " << to << ".\n";
        else
            std::cout << "  Illegal move: that would leave your king in check.\n";
        return;
    }

    bool promoting = piece->getType() == PieceType::Pawn && to.row == (turn_ == Color::White ? 7 : 0);
    char promo = 0;
    if (s.size() == 5) {
        PieceType t;
        if (!promoting) { std::cout << "  Only a promoting pawn takes a fifth letter.\n"; return; }
        if (!promotionTypeFromChar(s[4], t)) { std::cout << "  Promotion letter must be q, r, b or n.\n"; return; }
        promo = static_cast<char>(std::toupper(static_cast<unsigned char>(s[4])));
    } else if (promoting) {
        promo = askPromotion();
    }

    Move move(from, to, promo);
    saveSnapshot();
    std::string san = buildSAN(move);
    MoveInfo info = board_.makeMove(move);          // updates positions, captures, special moves

    if (info.movedType == PieceType::Pawn || info.capture) halfmoveClock_ = 0;
    else ++halfmoveClock_;
    if (turn_ == Color::Black) ++fullmoveNumber_;
    turn_ = opposite(turn_);

    bool inCheck = board_.isInCheck(turn_);
    bool canMove = board_.hasAnyLegalMove(turn_);
    if (inCheck) san += canMove ? "+" : "#";
    history_.push_back(san);

    std::cout << "  " << colorName(opposite(turn_)) << " plays " << san;
    if (info.enPassant) std::cout << "  (en passant capture)";
    else if (info.capture) std::cout << "  (captures a " << (info.capturedType == PieceType::Pawn ? "pawn" :
                                       info.capturedType == PieceType::Knight ? "knight" :
                                       info.capturedType == PieceType::Bishop ? "bishop" :
                                       info.capturedType == PieceType::Rook ? "rook" : "queen") << ")";
    if (info.castleKingSide || info.castleQueenSide) std::cout << "  (castling)";
    if (info.promotion) std::cout << "  (promotion)";
    std::cout << "\n";

    evaluateEnd(inCheck, canMove);
}

void Game::evaluateEnd(bool inCheck, bool canMove) {
    if (!canMove) {
        gameOver_ = true;
        if (inCheck) result_ = std::string("CHECKMATE! ") + colorName(opposite(turn_)) + " wins the game.";
        else result_ = "STALEMATE - " + std::string(colorName(turn_)) + " has no legal move. The game is a draw.";
        return;
    }
    if (board_.insufficientMaterial()) {
        gameOver_ = true;
        result_ = "Draw - neither side has enough material to checkmate.";
        return;
    }
    if (halfmoveClock_ >= 100) {
        gameOver_ = true;
        result_ = "Draw by the fifty-move rule (50 moves without a capture or pawn move).";
        return;
    }
    int seen = ++repetitions_[board_.positionKey(turn_)];
    if (seen >= 3) {
        gameOver_ = true;
        result_ = "Draw by threefold repetition.";
    }
}

// ----------------------------------------------------------------------------
void Game::run() {
    printBanner();
    std::string line;
    while (!gameOver_) {
        board_.display(unicode_, color_);
        printStatus();
        std::cout << "  " << colorName(turn_) << " > " << std::flush;
        if (!std::getline(std::cin, line)) {
            std::cout << "\n  Input closed. Goodbye!\n";
            return;
        }
        line = toLower(trim(line));
        if (line.empty()) continue;
        if (!handleCommand(line)) tryMove(line);
    }
    board_.display(unicode_, color_);
    std::cout << "  ==========================================\n"
              << "  " << result_ << "\n"
              << "  ==========================================\n";
    if (!history_.empty()) { std::cout << "\n  Game record:\n"; printHistory(); }
    std::cout << "\n";
}
