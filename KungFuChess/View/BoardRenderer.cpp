#include "BoardRenderer.h"
#include "GameOverOverlay.h"
#include "../GameConstants.h"
#include "../Input/BoardMapper.h"
#include <cmath>
#include <stdexcept>
#include <unordered_set>

BoardRenderer::BoardRenderer(std::string boardImagePath,
                             std::unique_ptr<IPieceSpriteProvider> sprites)
    : boardImagePath_(std::move(boardImagePath))
    , sprites_(std::move(sprites)) {
    if (!sprites_) {
        throw std::invalid_argument("BoardRenderer requires a piece sprite provider");
    }
    boardBackground_.read(boardImagePath_);
}

const DisplayMotion* BoardRenderer::findMotion(const BoardRenderHints& hints, int pieceId) {
    for (const DisplayMotion& motion : hints.motions) {
        if (motion.pieceId == pieceId) {
            return &motion;
        }
    }
    return nullptr;
}

const DisplayJump* BoardRenderer::findJump(const BoardRenderHints& hints, int pieceId) {
    for (const DisplayJump& jump : hints.jumps) {
        if (jump.pieceId == pieceId) {
            return &jump;
        }
    }
    return nullptr;
}

void BoardRenderer::drawPiece(Img& canvas,
                              const DisplayPiece& piece,
                              int cell,
                              int x,
                              int y,
                              long long animElapsed) const {
    Img sprite = sprites_->loadForCell(piece, cell, animElapsed);
    if (!sprite.is_loaded()) {
        return;
    }
    const int dx = (cell - sprite.width()) / Kfc::Ui::kSpriteCenterDivisor;
    const int dy = (cell - sprite.height()) / Kfc::Ui::kSpriteCenterDivisor;
    sprite.draw_on(canvas, x + dx, y + dy);
}

Img BoardRenderer::render(const DisplayBoard& board, const BoardRenderHints& hints) const {
    Img canvas = boardBackground_.clone();
    const int cell = BoardMapper::CELL_SIZE;
    for (const CellCoord& move : hints.legalMoves) {
        if (!board.inBounds(move)) {
            continue;
        }
        int x = 0;
        int y = 0;
        BoardMapper::cellToPixels(move, x, y);
        canvas.draw_rect(x + Kfc::Ui::kLegalMoveHighlightInsetPx,
                         y + Kfc::Ui::kLegalMoveHighlightInsetPx,
                         cell - Kfc::Ui::kLegalMoveHighlightShrinkPx,
                         cell - Kfc::Ui::kLegalMoveHighlightShrinkPx,
                         cv::Scalar(Kfc::Ui::Bgr::kLegalMoveHighlight[0],
                                    Kfc::Ui::Bgr::kLegalMoveHighlight[1],
                                    Kfc::Ui::Bgr::kLegalMoveHighlight[2]),
                         Kfc::Ui::kLegalMoveOutlineThicknessPx);
    }

    if (hints.selected.has_value() && board.inBounds(*hints.selected)) {
        int sx = 0;
        int sy = 0;
        BoardMapper::cellToPixels(*hints.selected, sx, sy);
        canvas.draw_rect(sx, sy, cell, cell,
                         cv::Scalar(Kfc::Ui::Bgr::kSelectionHighlight[0],
                                    Kfc::Ui::Bgr::kSelectionHighlight[1],
                                    Kfc::Ui::Bgr::kSelectionHighlight[2]),
                         Kfc::Ui::kSelectionOutlineThicknessPx);
    }

    std::unordered_set<int> drawnIds;

    for (int row = 0; row < board.rows(); ++row) {
        for (int col = 0; col < board.cols(); ++col) {
            const DisplayPiece* piece = board.pieceAt(CellCoord(row, col));
            if (piece == nullptr) {
                continue;
            }

            const DisplayMotion* motion = findMotion(hints, piece->id);
            const DisplayJump* jump = findJump(hints, piece->id);

            long long animElapsed = hints.clockMs;
            if (motion != nullptr) {
                animElapsed = hints.clockMs - motion->startMs;
            } else if (jump != nullptr) {
                animElapsed = hints.clockMs - jump->startMs;
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
                BoardMapper::cellToPixels(CellCoord(row, col), x, y);
                if (jump != nullptr) {
                    const double apex = Kfc::Progress::kComplete
                        - std::abs(Kfc::Ui::kJumpArcWaveScale * jump->progressAt(hints.clockMs)
                                   - Kfc::Progress::kComplete);
                    y -= static_cast<int>(static_cast<double>(Kfc::Ui::kJumpArcMaxLiftPx) * apex);
                }
            }

            drawPiece(canvas, *piece, cell, x, y, animElapsed);
            drawnIds.insert(piece->id);
        }
    }

    for (const DisplayMotion& motion : hints.motions) {
        if (drawnIds.count(motion.pieceId) != Kfc::Grid::kNeutral) {
            continue;
        }
        const DisplayPiece* piece = board.findById(motion.pieceId);
        if (piece == nullptr) {
            continue;
        }

        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;
        BoardMapper::cellToPixels(motion.src, x0, y0);
        BoardMapper::cellToPixels(motion.dst, x1, y1);
        const double t = motion.progressAt(hints.clockMs);
        const int x = static_cast<int>(x0 + (x1 - x0) * t);
        const int y = static_cast<int>(y0 + (y1 - y0) * t);
        drawPiece(canvas, *piece, cell, x, y, hints.clockMs - motion.startMs);
    }

    if (hints.gameOver) {
        GameOverOverlay::draw(canvas, hints.winner);
    }

    return canvas;
}

void BoardRenderer::show(const DisplayBoard& board, const BoardRenderHints& hints) const {
    render(board, hints).show();
}
