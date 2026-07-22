#pragma once

#include "../GameConstants.h"
#include <optional>
#include <vector>

struct CellCoord {
    int row = 0;
    int col = 0;

    CellCoord() = default;
    CellCoord(int row, int col) : row(row), col(col) {}

    bool operator==(const CellCoord& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const CellCoord& other) const { return !(*this == other); }
};

enum class DisplayColor { White, Black };
enum class DisplayKind { King, Queen, Rook, Bishop, Knight, Pawn };
enum class DisplayPieceState { Idle, Moving, Jumping, Captured };

struct DisplayPiece {
    int id = 0;
    DisplayColor color = DisplayColor::White;
    DisplayKind kind = DisplayKind::Pawn;
    CellCoord cell;
    DisplayPieceState state = DisplayPieceState::Idle;
};

struct DisplayMotion {
    int pieceId = 0;
    CellCoord src;
    CellCoord dst;
    long long startMs = 0;
    long long durationMs = 0;

    double progressAt(long long nowMs) const {
        return Kfc::Progress::fromElapsedDuration(nowMs - startMs, durationMs);
    }
};

struct DisplayJump {
    int pieceId = 0;
    CellCoord src;
    CellCoord dst;
    long long startMs = 0;
    long long durationMs = 0;

    double progressAt(long long nowMs) const {
        return Kfc::Progress::fromElapsedDuration(nowMs - startMs, durationMs);
    }
};

class DisplayBoard {
public:
    DisplayBoard() = default;
    DisplayBoard(int rows, int cols, std::vector<std::optional<DisplayPiece>> cells,
                 std::vector<DisplayPiece> byId);

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    bool inBounds(const CellCoord& pos) const;
    bool isEmpty(const CellCoord& pos) const;

    const DisplayPiece* pieceAt(const CellCoord& pos) const;
    const DisplayPiece* findById(int pieceId) const;
    const std::vector<DisplayPiece>& piecesById() const { return byId_; }

private:
    int rows_ = 0;
    int cols_ = 0;
    std::vector<std::optional<DisplayPiece>> cells_;
    std::vector<DisplayPiece> byId_;
};
