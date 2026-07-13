#pragma once
#include "../Model/Position.h"
#include <algorithm>
#include <cmath>

// MotionPath owns the GEOMETRY AND TIMING of a single trip: where it starts,
// where it ends, when it starts, how long it takes, and where the piece is at
// any instant in between.
//
// It is a pure value object. It never touches Board, never resolves captures,
// never decides legality, and never reads a clock - the caller passes the
// current time in. Arrival resolution belongs to RealTimeArbiter, not here.
class MotionPath {
public:
    // Fixed travel speed, expressed as the time it takes to cross one cell.
    static constexpr long long MS_PER_CELL = 1000;

    MotionPath(const Position& src, const Position& dst, long long startMs,
               long long msPerCell = MS_PER_CELL)
        : src_(src), dst_(dst), startMs_(startMs) {
        // Chebyshev distance: a diagonal step costs the same as a straight
        // step, which matches how chess pieces travel. A knight's L-jump is
        // charged as its longest leg.
        int dr = std::abs(dst.getRow() - src.getRow());
        int dc = std::abs(dst.getCol() - src.getCol());
        int cells = std::max(dr, dc);

        durationMs_ = static_cast<long long>(cells) * msPerCell;
        arrivalMs_  = startMs_ + durationMs_;
    }

    const Position& getSrc() const { return src_; }
    const Position& getDst() const { return dst_; }

    long long getStartMs() const { return startMs_; }
    long long getDurationMs() const { return durationMs_; }
    long long getArrivalMs() const { return arrivalMs_; }

    bool hasArrived(long long nowMs) const {
        return nowMs >= arrivalMs_;
    }

    // Fraction of the trip completed, clamped to [0.0, 1.0]. Used by the
    // renderer to draw a piece between two cells; the logical board still
    // shows the piece on src until arrival.
    double progress(long long nowMs) const {
        if (durationMs_ <= 0) return 1.0;
        double p = static_cast<double>(nowMs - startMs_) / static_cast<double>(durationMs_);
        return std::min(1.0, std::max(0.0, p));
    }

private:
    Position src_;
    Position dst_;
    long long startMs_;
    long long durationMs_;
    long long arrivalMs_;
};
