#include "Types.h"
#include <cctype>
#include <ostream>

std::string Position::toString() const {
    if (!isValid()) return "??";
    std::string s;
    s += static_cast<char>('a' + col);
    s += static_cast<char>('1' + row);
    return s;
}

Position Position::fromString(const std::string& text) {
    if (text.size() != 2) return Position();
    int c = std::tolower(static_cast<unsigned char>(text[0])) - 'a';
    int r = text[1] - '1';
    Position p(r, c);
    return p.isValid() ? p : Position();
}

std::ostream& operator<<(std::ostream& os, const Position& p) {
    return os << p.toString();
}
