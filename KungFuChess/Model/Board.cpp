#include "Board.h"

Board::Board() : rows_(0), cols_(0) {}

Board::Board(int rows, int cols) : rows_(rows), cols_(cols) {
    grid_.assign(rows, std::vector<Piece*>(cols, nullptr));
}

int Board::getRows() const { return rows_; }
int Board::getCols() const { return cols_; }

bool Board::inBounds(const Position& pos) const {
    return pos.getRow() >= 0 && pos.getRow() < rows_ &&
        pos.getCol() >= 0 && pos.getCol() < cols_;
}

bool Board::isEmpty(const Position& pos) const {
    if (!inBounds(pos)) return true;
    return grid_[pos.getRow()][pos.getCol()] == nullptr;
}

bool Board::addPiece(std::unique_ptr<Piece> piece) {
    if (!piece) return false;

    Position cell = piece->getCell();
    if (!inBounds(cell) || !isEmpty(cell)) return false;

    for (const auto& existing : pieces_) {
        if (existing->getId() == piece->getId()) return false; // duplicate id
    }

    grid_[cell.getRow()][cell.getCol()] = piece.get();
    pieces_.push_back(std::move(piece));
    return true;
}

bool Board::removePiece(const Position& pos) {
    if (!inBounds(pos) || isEmpty(pos)) return false;

    Piece* piece = grid_[pos.getRow()][pos.getCol()];
    piece->setState(PieceState::Captured);
    grid_[pos.getRow()][pos.getCol()] = nullptr;
    return true;
}

void Board::detachPiece(Piece* piece) {
    if (piece == nullptr) {
        return;
    }
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            if (grid_[row][col] == piece) {
                grid_[row][col] = nullptr;
            }
        }
    }
}

bool Board::placePiece(Piece* piece, const Position& dest) {
    if (piece == nullptr || !inBounds(dest) || !isEmpty(dest)) {
        return false;
    }
    detachPiece(piece);
    grid_[dest.getRow()][dest.getCol()] = piece;
    piece->setCell(dest);
    return true;
}

bool Board::removePieceById(int pieceId) {
    Piece* piece = findPieceById(pieceId);
    if (piece == nullptr) {
        return false;
    }
    detachPiece(piece);
    piece->setState(PieceState::Captured);
    return true;
}

const Piece* Board::getPieceAt(const Position& pos) const {
    if (!inBounds(pos)) return nullptr;
    return grid_[pos.getRow()][pos.getCol()];
}

Piece* Board::getPieceAt(const Position& pos) {
    if (!inBounds(pos)) return nullptr;
    return grid_[pos.getRow()][pos.getCol()];
}

bool Board::isFriendly(const Position& a, const Position& b) const {
    const Piece* pa = getPieceAt(a);
    const Piece* pb = getPieceAt(b);
    if (pa == nullptr || pb == nullptr) return false;
    return pa->getColor() == pb->getColor();
}

void Board::movePiece(const Position& src, const Position& dst) {
    if (!inBounds(src) || !inBounds(dst)) return;

    Piece* moving = grid_[src.getRow()][src.getCol()];
    if (moving == nullptr) return; 
    Piece* captured = grid_[dst.getRow()][dst.getCol()];
    if (captured != nullptr) {
        captured->setState(PieceState::Captured);
    }

    grid_[dst.getRow()][dst.getCol()] = moving;
    grid_[src.getRow()][src.getCol()] = nullptr;
    moving->setCell(dst);
}

Piece* Board::findPieceById(int pieceId) {
    for (auto& piece : pieces_) {
        if (piece && piece->getId() == pieceId && piece->getState() != PieceState::Captured) {
            return piece.get();
        }
    }
    return nullptr;
}

const Piece* Board::findPieceById(int pieceId) const {
    for (const auto& piece : pieces_) {
        if (piece && piece->getId() == pieceId && piece->getState() != PieceState::Captured) {
            return piece.get();
        }
    }
    return nullptr;
}
