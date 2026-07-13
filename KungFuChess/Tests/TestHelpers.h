#pragma once
// Shared fixtures used across the test files.

#include "../Model/Board.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include "../Rules/RuleEngine.h"
#include <memory>
#include <string>

// Unique piece ids across the whole test run.
inline int& nextPieceId() { static int id = 1; return id; }

// Place a piece on the board. Ids are handed out automatically.
inline void put(Board& board, PieceColor color, PieceKind kind, int row, int col) {
    board.addPiece(std::make_unique<Piece>(nextPieceId()++, color, kind, Position(row, col)));
}

// The reason RuleEngine gives for a move: "ok", "outside_board",
// "empty_source", "friendly_destination", or "illegal_piece_move".
inline std::string why(const Board& board, Position src, Position dst) {
    return RuleEngine::validateMove(board, src, dst).reason;
}

inline bool legal(const Board& board, Position src, Position dst) {
    return RuleEngine::validateMove(board, src, dst).is_valid;
}

// Centre pixel of cell index n, for click tests (BoardMapper::CELL_SIZE == 100).
inline int px(int index) { return index * 100 + 50; }
