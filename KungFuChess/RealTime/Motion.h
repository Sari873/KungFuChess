#pragma once
#include "MotionPath.h"
#include "../Model/Piece.h"

struct Motion {
    int pieceId;
    PieceColor color;
    MotionPath path;

    Motion(int pieceId, PieceColor color, const MotionPath& path)
        : pieceId(pieceId), color(color), path(path) {
    }

    bool hasArrived(long long nowMs) const {
        return path.hasArrived(nowMs);
    }

    long long getArrivalMs() const {
        return path.getArrivalMs();
    }
};
