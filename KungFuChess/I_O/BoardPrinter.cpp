#include "BoardPrinter.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include <iostream>

void BoardPrinter::print(const Board& board, std::ostream& out) {
    for (int r = 0; r < board.getRows(); ++r) {
        for (int c = 0; c < board.getCols(); ++c) {
            Position pos(r, c);
            const Piece* piece = board.getPieceAt(pos);
            out << (piece != nullptr ? piece->toNotation() : ".");
            if (c != board.getCols() - 1) out << ' ';
        }
        out << '\n';
    }
}

void BoardPrinter::print(const Board& board) {
    print(board, std::cout);
}