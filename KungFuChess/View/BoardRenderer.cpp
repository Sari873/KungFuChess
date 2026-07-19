#include "BoardRenderer.h"
#include "GameOverOverlay.h"
#include "../Input/BoardMapper.h"
#include <stdexcept>

BoardRenderer::BoardRenderer(std::string boardImagePath,
                             std::unique_ptr<IPieceSpriteProvider> sprites)
    : boardImagePath_(std::move(boardImagePath))
    , sprites_(std::move(sprites)) {
    if (!sprites_) {
        throw std::invalid_argument("BoardRenderer requires a piece sprite provider");
    }
    boardBackground_.read(boardImagePath_);
}

BoardRenderer::MotionIndex BoardRenderer::indexMotions(const BoardRenderHints& hints) {
    MotionIndex index;
    index.reserve(hints.motions.size());
    for (const MotionSnapshot& motion : hints.motions) {
        index.emplace(motion.pieceId, &motion);
    }
    return index;
}

Img BoardRenderer::render(const Board& board, const BoardRenderHints& hints) const {
    Img canvas = boardBackground_.clone();
    const int cell = BoardMapper::CELL_SIZE;
    const MotionIndex motions = indexMotions(hints);

    for (const Position& move : hints.legalMoves) {
        if (!board.inBounds(move)) {
            continue;
        }
        int x = 0;
        int y = 0;
        BoardMapper::cellToPixels(move, x, y);
        canvas.draw_rect(x + 8, y + 8, cell - 16, cell - 16, cv::Scalar(80, 200, 80), 2);
    }

    if (hints.selected.has_value() && board.inBounds(*hints.selected)) {
        int sx = 0;
        int sy = 0;
        BoardMapper::cellToPixels(*hints.selected, sx, sy);
        canvas.draw_rect(sx, sy, cell, cell, cv::Scalar(0, 220, 255), 3);
    }

    for (int row = 0; row < board.getRows(); ++row) {
        for (int col = 0; col < board.getCols(); ++col) {
            const Piece* piece = board.getPieceAt(Position(row, col));
            if (piece == nullptr) {
                continue;
            }

            const MotionSnapshot* motion = nullptr;
            const auto motionIt = motions.find(piece->getId());
            if (motionIt != motions.end()) {
                motion = motionIt->second;
            }

            const long long animElapsed = motion != nullptr
                ? (hints.clockMs - motion->startMs)
                : hints.clockMs;

            Img sprite = sprites_->loadForCell(*piece, cell, animElapsed);
            if (!sprite.is_loaded()) {
                continue;
            }

            int x = 0;
            int y = 0;
            if (motion != nullptr) {
                int x0 = 0;
                int y0 = 0;
                int x1 = 0;
                int y1 = 0;
                BoardMapper::cellToPixels(motion->src, x0, y0);
                BoardMapper::cellToPixels(motion->dst, x1, y1);
                const double t = motion->progressAt(hints.clockMs);
                x = static_cast<int>(x0 + (x1 - x0) * t);
                y = static_cast<int>(y0 + (y1 - y0) * t);
            } else {
                BoardMapper::cellToPixels(Position(row, col), x, y);
            }

            const int dx = (cell - sprite.width()) / 2;
            const int dy = (cell - sprite.height()) / 2;
            sprite.draw_on(canvas, x + dx, y + dy);
        }
    }

    if (hints.gameOver) {
        GameOverOverlay::draw(canvas, hints.winner);
    }

    return canvas;
}

void BoardRenderer::show(const Board& board, const BoardRenderHints& hints) const {
    render(board, hints).show();
}
