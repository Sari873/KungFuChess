#pragma once
// Shared fixtures used across the test files.

#include "../GameConstants.h"
#include "../Input/BoardMapper.h"
#include "../Model/Board.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include "../Rules/RuleEngine.h"
#include <memory>
#include <string>

inline int& nextPieceId() {
    static int id = Kfc::Limits::kFirstAutoPieceId;
    return id;
}

inline void put(Board& board, PieceColor color, PieceKind kind, int row, int col) {
    board.addPiece(std::make_unique<Piece>(nextPieceId()++, color, kind, Position(row, col)));
}

inline std::string why(const Board& board, Position src, Position dst) {
    return RuleEngine::validateMove(board, src, dst).reason;
}

inline bool legal(const Board& board, Position src, Position dst) {
    return RuleEngine::validateMove(board, src, dst).is_valid;
}

inline int px(int index) {
    return index * BoardMapper::CELL_SIZE + Kfc::Ui::kCellCenterPixelOffset;
}
