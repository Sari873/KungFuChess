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

// RealTimeArbiter owns movement over time.
//
// Per the layer table it owns: active Motion objects, simulated time
// advancement, arrival resolution, and capture events.
// It must NOT own: chess legality, clicks, rendering, or script parsing.
//
// This is the ONLY class that mutates the board as a result of time passing.
// A move that has been validated by RuleEngine is handed here as a motion; the
// piece stays on its source cell until the arrival time is reached.
class RealTimeArbiter {
public:
    // Begins a motion for the piece standing on 'src'. Assumes the move was
    // already validated by RuleEngine. Returns false if that piece is already
    // in flight.
    bool startMotion(Board& board, const Position& src, const Position& dst, long long nowMs);

    // Advances simulated time to nowMs, applying every motion that has arrived
    // (in arrival order) and resolving captures. Returns the captures that
    // happened during this advance.
    std::vector<CaptureEvent> advanceTo(long long nowMs, Board& board);

    bool isPieceMoving(int pieceId) const;
    bool isMoving(const Board& board, const Position& cell) const;
    bool hasActiveMotions() const;

private:
    std::vector<Motion> active_;
};
