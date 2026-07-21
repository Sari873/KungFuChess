#include "../GameConstants.h"
#include "DisplayTypes.h"

DisplayBoard::DisplayBoard(int rows, int cols, std::vector<std::optional<DisplayPiece>> cells,
                           std::vector<DisplayPiece> byId)
    : rows_(rows), cols_(cols), cells_(std::move(cells)), byId_(std::move(byId)) {}

bool DisplayBoard::inBounds(const CellCoord& pos) const {
    return pos.row >= Kfc::Grid::kNeutral && pos.row < rows_
        && pos.col >= Kfc::Grid::kNeutral && pos.col < cols_;
}

bool DisplayBoard::isEmpty(const CellCoord& pos) const {
    if (!inBounds(pos)) {
        return true;
    }
    const std::size_t index = static_cast<std::size_t>(pos.row * cols_ + pos.col);
    if (index >= cells_.size()) {
        return true;
    }
    return !cells_[index].has_value();
}

const DisplayPiece* DisplayBoard::pieceAt(const CellCoord& pos) const {
    if (!inBounds(pos)) {
        return nullptr;
    }
    const std::size_t index = static_cast<std::size_t>(pos.row * cols_ + pos.col);
    if (index >= cells_.size() || !cells_[index].has_value()) {
        return nullptr;
    }
    return &*cells_[index];
}

const DisplayPiece* DisplayBoard::findById(int pieceId) const {
    for (const DisplayPiece& piece : byId_) {
        if (piece.id == pieceId) {
            return &piece;
        }
    }
    return nullptr;
}
