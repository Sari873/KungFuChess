#pragma once
#include "../Model/Board.h"
#include <iosfwd>

class BoardPrinter {
public:
    static void print(const Board& board, std::ostream& out);
    static void print(const Board& board);
};