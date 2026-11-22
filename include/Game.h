// ============================================================================
//  Game.h  -  Controls the match: turns, input, undo, end-of-game detection
// ============================================================================
#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <map>
#include <string>
#include <vector>

class Game {
public:
    Game(bool unicode = false, bool color = false);
    bool loadFEN(const std::string& fen);   // start from a custom position
    void run();                              // main game loop

private:
    // A saved copy of the whole game state (used by "undo")
    struct Snapshot {
        Board board;
        Color turn;
        int halfmoveClock;
        int fullmoveNumber;
        std::vector<std::string> history;
        std::map<std::string, int> repetitions;
    };

    Board board_;
    Color turn_;
    int halfmoveClock_;
    int fullmoveNumber_;
    Color startTurn_;
    int startFullmove_;
    std::vector<std::string> history_;
    std::map<std::string, int> repetitions_;
    std::vector<Snapshot> undoStack_;
    bool unicode_;
    bool color_;
    bool gameOver_;
    std::string result_;

    void resetTracking();
    void printBanner() const;
    void printHelp() const;
    void printStatus() const;
    void printHistory() const;
    bool handleCommand(const std::string& line);
    void tryMove(const std::string& line);
    void showMoves(const std::string& square) const;
    void saveSnapshot();
    bool undo();
    char askPromotion() const;
    std::string buildSAN(const Move& move) const;
    void evaluateEnd(bool inCheck, bool canMove);
};

#endif // GAME_H
