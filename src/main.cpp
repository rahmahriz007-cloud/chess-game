// ============================================================================
//  main.cpp  -  Program entry point
// ============================================================================
#include "Demo.h"
#include "Game.h"
#include "Piece.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
static void enableAnsiAndUtf8() {
    SetConsoleOutputCP(65001);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode)) SetConsoleMode(h, mode | 0x0004);   // virtual terminal
}
#else
static void enableAnsiAndUtf8() {}
#endif

static void printUsage() {
    std::cout << "Usage: chess [options]\n"
              << "  --color       coloured board (needs an ANSI terminal)\n"
              << "  --unicode     draw pieces with chess symbols\n"
              << "  --fen \"...\"   start from a position given in FEN notation\n"
              << "  --trace       print constructor / destructor messages while playing\n"
              << "  --demo        show a guided tour of the OOP features and exit\n"
              << "  --help        show this text\n";
}

int main(int argc, char* argv[]) {
    bool unicode = false, color = false, demo = false, trace = false;
    std::string fen;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--unicode") unicode = true;
        else if (arg == "--color") color = true;
        else if (arg == "--trace") trace = true;
        else if (arg == "--demo") demo = true;
        else if (arg == "--help" || arg == "-h") { printUsage(); return 0; }
        else if (arg == "--fen") {
            // accept both  --fen "a b c"  and  --fen a b c
            while (i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) != 0) {
                if (!fen.empty()) fen += ' ';
                fen += argv[++i];
            }
        } else {
            std::cout << "Unknown option: " << arg << "\n";
            printUsage();
            return 1;
        }
    }

    if (color || unicode) enableAnsiAndUtf8();

    if (demo) { runDemo(); return 0; }

    Piece::setTrace(trace);
    Game game(unicode, color);
    if (!fen.empty() && !game.loadFEN(fen)) {
        std::cout << "That FEN string is not valid.\n";
        return 1;
    }
    game.run();
    return 0;
}
