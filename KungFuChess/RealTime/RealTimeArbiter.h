#pragma once
#include "../Model/Board.h"
#include "../Model/Position.h"
#include "Motion.h"
#include <vector>

// A capture that happened at arrival time. Reported upward so GameEngine can
// react (e.g. end the game on king capture) without RealTimeArbiter knowing
// what game-over means.
struct CaptureEvent {
    int capturedPieceId;
    PieceKind capturedKind;
    PieceColor capturedColor;
    Position cell;
};

class RealTimeArbiter {
public:
    bool startMotion(Board& board, const Position& src, const Position& dst, long long nowMs);

    std::vector<CaptureEvent> advanceTo(long long nowMs, Board& board);

    bool isPieceMoving(int pieceId) const;
    bool isMoving(const Board& board, const Position& cell) const;
    bool hasActiveMotions() const;

private:
    std::vector<Motion> active_;
};
