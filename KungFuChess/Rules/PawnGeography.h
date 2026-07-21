#pragma once
#include "../GameConstants.h"
#include "../Model/Board.h"
#include "../Model/Piece.h"

namespace PawnGeography {

inline int forwardStep(PieceColor color) {
    return color == PieceColor::White ? Kfc::Grid::kBackward : Kfc::Grid::kForward;
}

inline int startRow(PieceColor color, const Board& board) {
    return color == PieceColor::White ? board.getRows() - Kfc::Grid::kForward : Kfc::Grid::kNeutral;
}

inline int lastRow(PieceColor color, const Board& board) {
    return color == PieceColor::White ? Kfc::Grid::kNeutral : board.getRows() - Kfc::Grid::kForward;
}

} 
