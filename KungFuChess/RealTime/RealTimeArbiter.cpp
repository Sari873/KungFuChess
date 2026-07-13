#include "RealTimeArbiter.h"
#include "../Model/Piece.h"
#include <algorithm>

bool RealTimeArbiter::isPieceMoving(int pieceId) const {
    for (const Motion& m : active_) {
        if (m.pieceId == pieceId) return true;
    }
    return false;
}

bool RealTimeArbiter::isMoving(const Board& board, const Position& cell) const {
    const Piece* piece = board.getPieceAt(cell);
    if (piece == nullptr) return false;
    return isPieceMoving(piece->getId());
}

bool RealTimeArbiter::hasActiveMotions() const {
    return !active_.empty();
}

bool RealTimeArbiter::startMotion(Board& board, const Position& src, const Position& dst, long long nowMs) {
    Piece* piece = board.getPieceAt(src);
    if (piece == nullptr) return false;

    // A piece already in flight cannot accept a second command.
    if (isPieceMoving(piece->getId())) return false;

    // The board does NOT change here. The piece stays on src and is flagged
    // Moving, so `print board` still shows it at its original cell until the
    // arrival time is reached.
    piece->setState(PieceState::Moving);
    active_.emplace_back(piece->getId(), MotionPath(src, dst, nowMs));
    return true;
}

std::vector<CaptureEvent> RealTimeArbiter::advanceTo(long long nowMs, Board& board) {
    std::vector<CaptureEvent> captures;

    std::vector<Motion> arrived;
    std::vector<Motion> stillFlying;

    for (const Motion& m : active_) {
        if (m.hasArrived(nowMs)) {
            arrived.push_back(m);
        }
        else {
            stillFlying.push_back(m);
        }
    }

    // Resolve arrivals in arrival order, so two motions landing inside the same
    // advanceTo() call settle deterministically: the earlier one lands first.
    std::stable_sort(arrived.begin(), arrived.end(),
        [](const Motion& a, const Motion& b) {
            return a.getArrivalMs() < b.getArrivalMs();
        });

    for (const Motion& m : arrived) {
        const Position& src = m.path.getSrc();
        const Position& dst = m.path.getDst();

        Piece* moving = board.getPieceAt(src);

        // The piece may have been captured mid-flight, or an earlier arrival in
        // this same batch may have already changed the source cell.
        if (moving == nullptr || moving->getId() != m.pieceId) {
            continue;
        }

        const Piece* target = board.getPieceAt(dst);
        if (target != nullptr && target->getColor() == moving->getColor()) {
            // A friendly piece got there first. The move is abandoned and the
            // piece simply stays where it is.
            moving->setState(PieceState::Idle);
            continue;
        }

        if (target != nullptr) {
            // Record the capture BEFORE movePiece marks it Captured.
            captures.push_back(CaptureEvent{
                target->getId(),
                target->getKind(),
                target->getColor(),
                dst
                });
        }

        board.movePiece(src, dst);
        moving->setState(PieceState::Idle);
    }

    active_ = stillFlying;
    return captures;
}
