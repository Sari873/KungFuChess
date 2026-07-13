#pragma once
#include "MotionPath.h"

// Motion binds ONE piece to ONE MotionPath: "piece #7 is travelling along
// this path".
//
// The split is deliberate. MotionPath answers "where and when"; Motion answers
// "who". Neither of them answers "what happens on arrival" - that is
// RealTimeArbiter's job, and it is why neither class knows what a Board is.
//
// This is also why Piece.state stays a bare lifecycle flag: the path,
// destination, elapsed time, speed, and interpolation all live here instead.
struct Motion {
    int pieceId;
    MotionPath path;

    Motion(int pieceId, const MotionPath& path)
        : pieceId(pieceId), path(path) {}

    bool hasArrived(long long nowMs) const {
        return path.hasArrived(nowMs);
    }

    long long getArrivalMs() const {
        return path.getArrivalMs();
    }
};
