#pragma once
#include "MotionPath.h"

struct Motion {
    int pieceId;
    MotionPath path;

    Motion(int pieceId, const MotionPath& path)
        : pieceId(pieceId), path(path) {
    }

    bool hasArrived(long long nowMs) const {
        return path.hasArrived(nowMs);
    }

    long long getArrivalMs() const {
        return path.getArrivalMs();
    }
};
