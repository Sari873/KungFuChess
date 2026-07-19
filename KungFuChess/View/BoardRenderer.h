#pragma once
#include "../Model/Board.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include "../RealTime/RealTimeArbiter.h"
#include "IPieceSpriteProvider.h"
#include "img.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct BoardRenderHints {
    std::optional<Position> selected;
    std::vector<Position> legalMoves;
    long long clockMs = 0;
    std::vector<MotionSnapshot> motions;
    bool gameOver = false;
    std::optional<PieceColor> winner;
};


class BoardRenderer {
public:
    BoardRenderer(std::string boardImagePath, std::unique_ptr<IPieceSpriteProvider> sprites);

    Img render(const Board& board, const BoardRenderHints& hints = {}) const;
    void show(const Board& board, const BoardRenderHints& hints = {}) const;

private:
    using MotionIndex = std::unordered_map<int, const MotionSnapshot*>;

    static MotionIndex indexMotions(const BoardRenderHints& hints);

    std::string boardImagePath_;
    Img boardBackground_;
    std::unique_ptr<IPieceSpriteProvider> sprites_;
};
