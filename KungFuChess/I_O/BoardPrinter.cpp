#include "BoardPrinter.h"
#include "Piece.h"
#include "Position.h"
#include <iostream>

void BoardPrinter::print(const Board& board) {
    for (int r = 0; r < board.getRows(); ++r) {
        for (int c = 0; c < board.getCols(); ++c) {
            Position pos(r, c);
            const Piece* piece = board.getPieceAt(pos);
            std::cout << (piece != nullptr ? piece->toNotation() : ".");
            if (c != board.getCols() - 1) std::cout << ' ';
        }
        std::cout << '\n';
    }
}
