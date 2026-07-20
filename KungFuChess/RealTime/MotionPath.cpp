#include "MotionPath.h"

std::vector<Position> MotionPath::buildWaypoints(const Position& src, const Position& dst) {
    const int dr = dst.getRow() - src.getRow();
    const int dc = dst.getCol() - src.getCol();
    const int absDr = std::abs(dr);
    const int absDc = std::abs(dc);
    const int steps = std::max(absDr, absDc);

    if (steps == 0) {
        return {};
    }

    const bool isStraightOrDiagonal = (dr == 0 || dc == 0 || absDr == absDc);
    if (!isStraightOrDiagonal) {
        return { dst };
    }

    const int stepR = (dr == 0) ? 0 : dr / absDr;
    const int stepC = (dc == 0) ? 0 : dc / absDc;

    std::vector<Position> points;
    points.reserve(static_cast<std::size_t>(steps));
    for (int i = 1; i <= steps; ++i) {
        points.emplace_back(src.getRow() + i * stepR, src.getCol() + i * stepC);
    }
    return points;
}

MotionPath::MotionPath(const Position& src, const Position& dst, long long startMs,
                       long long msPerCell)
    : src_(src)
    , dst_(dst)
    , startMs_(startMs)
    , msPerCell_(msPerCell)
    , waypoints_(buildWaypoints(src, dst)) {
    const int dr = std::abs(dst.getRow() - src.getRow());
    const int dc = std::abs(dst.getCol() - src.getCol());
    const int cells = std::max(dr, dc);
    durationMs_ = static_cast<long long>(cells) * msPerCell_;
    arrivalMs_ = startMs_ + durationMs_;
}

double MotionPath::progress(long long nowMs) const {
    if (durationMs_ <= 0) return 1.0;
    double p = static_cast<double>(nowMs - startMs_) / static_cast<double>(durationMs_);
    return std::min(1.0, std::max(0.0, p));
}

long long MotionPath::enterTime(std::size_t waypointIndex) const {
    if (waypoints_.empty()) {
        return arrivalMs_;
    }
    if (waypoints_.size() == 1) {
        return arrivalMs_;
    }
    return startMs_ + static_cast<long long>(waypointIndex + 1) * msPerCell_;
}

int MotionPath::waypointIndexAtEnterTime(long long timeMs) const {
    for (std::size_t i = 0; i < waypoints_.size(); ++i) {
        if (enterTime(i) == timeMs) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

Position MotionPath::previousCell(std::size_t waypointIndex) const {
    if (waypointIndex == 0) {
        return src_;
    }
    return waypoints_[waypointIndex - 1];
}

Position MotionPath::cellAt(long long nowMs) const {
    if (waypoints_.empty() || nowMs < enterTime(0)) {
        return src_;
    }
    if (nowMs >= arrivalMs_) {
        return dst_;
    }

    Position current = src_;
    for (std::size_t i = 0; i < waypoints_.size(); ++i) {
        if (nowMs >= enterTime(i)) {
            current = waypoints_[i];
        } else {
            break;
        }
    }
    return current;
}

bool MotionPath::occupies(const Position& cell, long long nowMs) const {
    if (nowMs < startMs_) {
        return false;
    }

    // Once motion starts the piece has left the origin; it is only present on
    // the discrete path cells it has actually entered.
    for (std::size_t i = 0; i < waypoints_.size(); ++i) {
        if (waypoints_[i] != cell) {
            continue;
        }
        const long long enter = enterTime(i);
        const long long leave = (i + 1 < waypoints_.size()) ? enterTime(i + 1) : arrivalMs_;
        if (nowMs >= enter && nowMs <= leave) {
            return true;
        }
    }
    return false;
}

void MotionPath::truncateTo(const Position& newDst, long long newArrivalMs) {
    dst_ = newDst;
    arrivalMs_ = newArrivalMs;
    durationMs_ = std::max(0LL, arrivalMs_ - startMs_);

    if (newDst == src_) {
        waypoints_.clear();
        return;
    }

    std::vector<Position> kept;
    for (const Position& cell : waypoints_) {
        kept.push_back(cell);
        if (cell == newDst) {
            break;
        }
    }
    if (kept.empty() || kept.back() != newDst) {
        kept.push_back(newDst);
    }
    waypoints_ = std::move(kept);
}
