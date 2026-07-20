#pragma once
#include "../Model/Board.h"
#include "../Model/Position.h"
#include "Jump.h"
#include "Motion.h"
#include <optional>
#include <vector>

struct CaptureEvent {
    int capturedPieceId;
    PieceKind capturedKind;
    PieceColor capturedColor;
    Position cell;
};

struct ArrivalEvent {
    int pieceId;
    Position cell;
};

struct ArrivalReport {
    std::vector<CaptureEvent> captures;
    std::vector<ArrivalEvent> arrivals;
};

struct MotionSnapshot {
    int pieceId = 0;
    Position src;
    Position dst;
    long long startMs = 0;
    long long arrivalMs = 0;

    double progressAt(long long nowMs) const {
        const long long duration = arrivalMs - startMs;
        if (duration <= 0) {
            return 1.0;
        }
        const double p = static_cast<double>(nowMs - startMs) / static_cast<double>(duration);
        if (p < 0.0) return 0.0;
        if (p > 1.0) return 1.0;
        return p;
    }
};


class RealTimeArbiter {
public:
    bool startMotion(Board& board, const Position& src, const Position& dst, long long nowMs);
    bool startJump(Board& board, const Position& cell, long long nowMs);

    ArrivalReport advanceTo(long long nowMs, Board& board);

    bool isPieceMoving(int pieceId) const;
    bool isPieceJumping(int pieceId) const;
    bool isMoving(const Board& board, const Position& cell) const;
    bool isJumping(const Board& board, const Position& cell) const;
    bool hasActiveMotions() const;
    bool hasActiveJumps() const;

    std::vector<MotionSnapshot> snapshots() const;
    std::optional<MotionSnapshot> snapshotForPiece(int pieceId) const;
    std::vector<JumpSnapshot> jumpSnapshots() const;

private:
    struct Occupant {
        int pieceId = 0;
        PieceColor color = PieceColor::White;
        long long presentSinceMs = 0;
    };

    struct EntryEvent {
        long long timeMs = 0;
        int pieceId = 0;
        std::size_t waypointIndex = 0;
        Position cell;
    };

    Motion* findMotion(int pieceId);
    const Motion* findMotion(int pieceId) const;
    void removeMotion(int pieceId);

    Jump* findJump(int pieceId);
    const Jump* findJump(int pieceId) const;
    void removeJump(int pieceId);
    void landJump(int pieceId, Board& board);

    std::optional<Occupant> findEarlierOccupant(int enteringPieceId, const Position& cell,
                                               long long timeMs, const Board& board) const;

    void landPiece(int pieceId, const Position& dest, Board& board, ArrivalReport& report);
    void capturePiece(int pieceId, const Position& cell, Board& board, ArrivalReport& report);
    void resolveEntry(const EntryEvent& event, Board& board, ArrivalReport& report);

    std::vector<Motion> active_;
    std::vector<Jump> jumps_;
    long long lastAdvanceMs_ = -1;
};
