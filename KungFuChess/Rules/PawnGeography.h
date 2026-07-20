#pragma once
#include "../Model/Board.h"
#include "../Model/Piece.h"

namespace PawnGeography {

inline int forwardStep(PieceColor color) {
    return color == PieceColor::White ? -1 : 1;
}

inline int startRow(PieceColor color, const Board& board) {
    return color == PieceColor::White ? board.getRows() - 1 : 0;
}

inline int lastRow(PieceColor color, const Board& board) {
    return color == PieceColor::White ? 0 : board.getRows() - 1;
}

} 
