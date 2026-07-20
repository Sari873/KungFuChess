#include "RealTimeArbiter.h"
#include "CollisionRules.h"
#include "JumpRules.h"
#include "../Model/Piece.h"
#include <algorithm>
#include <limits>

bool RealTimeArbiter::isPieceMoving(int pieceId) const {
    return findMotion(pieceId) != nullptr;
}

bool RealTimeArbiter::isPieceJumping(int pieceId) const {
    return findJump(pieceId) != nullptr;
}

bool RealTimeArbiter::isMoving(const Board& board, const Position& cell) const {
    const Piece* piece = board.getPieceAt(cell);
    if (piece == nullptr) return false;
    return isPieceMoving(piece->getId());
}

bool RealTimeArbiter::isJumping(const Board& board, const Position& cell) const {
    const Piece* piece = board.getPieceAt(cell);
    if (piece == nullptr) return false;
    return isPieceJumping(piece->getId());
}

bool RealTimeArbiter::hasActiveMotions() const {
    return !active_.empty();
}

bool RealTimeArbiter::hasActiveJumps() const {
    return !jumps_.empty();
}

Motion* RealTimeArbiter::findMotion(int pieceId) {
    for (Motion& motion : active_) {
        if (motion.pieceId == pieceId) {
            return &motion;
        }
    }
    return nullptr;
}

const Motion* RealTimeArbiter::findMotion(int pieceId) const {
    for (const Motion& motion : active_) {
        if (motion.pieceId == pieceId) {
            return &motion;
        }
    }
    return nullptr;
}

void RealTimeArbiter::removeMotion(int pieceId) {
    active_.erase(std::remove_if(active_.begin(), active_.end(),
                                 [pieceId](const Motion& m) { return m.pieceId == pieceId; }),
                  active_.end());
}

Jump* RealTimeArbiter::findJump(int pieceId) {
    for (Jump& jump : jumps_) {
        if (jump.pieceId == pieceId) {
            return &jump;
        }
    }
    return nullptr;
}

const Jump* RealTimeArbiter::findJump(int pieceId) const {
    for (const Jump& jump : jumps_) {
        if (jump.pieceId == pieceId) {
            return &jump;
        }
    }
    return nullptr;
}

void RealTimeArbiter::removeJump(int pieceId) {
    jumps_.erase(std::remove_if(jumps_.begin(), jumps_.end(),
                                [pieceId](const Jump& j) { return j.pieceId == pieceId; }),
                 jumps_.end());
}

void RealTimeArbiter::landJump(int pieceId, Board& board) {
    Piece* piece = board.findPieceById(pieceId);
    removeJump(pieceId);
    if (piece != nullptr && piece->getState() == PieceState::Jumping) {
        piece->setState(PieceState::Idle);
    }
}

std::vector<MotionSnapshot> RealTimeArbiter::snapshots() const {
    std::vector<MotionSnapshot> out;
    out.reserve(active_.size());
    for (const Motion& m : active_) {
        out.push_back(MotionSnapshot{
            m.pieceId,
            m.path.getSrc(),
            m.path.getDst(),
            m.path.getStartMs(),
            m.path.getArrivalMs()
        });
    }
    return out;
}

std::optional<MotionSnapshot> RealTimeArbiter::snapshotForPiece(int pieceId) const {
    const Motion* motion = findMotion(pieceId);
    if (motion == nullptr) {
        return std::nullopt;
    }
    return MotionSnapshot{
        motion->pieceId,
        motion->path.getSrc(),
        motion->path.getDst(),
        motion->path.getStartMs(),
        motion->path.getArrivalMs()
    };
}

std::vector<JumpSnapshot> RealTimeArbiter::jumpSnapshots() const {
    std::vector<JumpSnapshot> out;
    out.reserve(jumps_.size());
    for (const Jump& jump : jumps_) {
        out.push_back(JumpSnapshot{
            jump.pieceId,
            jump.cell,
            jump.startMs,
            jump.landMs()
        });
    }
    return out;
}

bool RealTimeArbiter::startMotion(Board& board, const Position& src, const Position& dst, long long nowMs) {
    Piece* piece = board.getPieceAt(src);
    if (piece == nullptr) return false;
    if (isPieceMoving(piece->getId()) || isPieceJumping(piece->getId())) return false;
    if (piece->getState() != PieceState::Idle) return false;

    piece->setState(PieceState::Moving);
    // Leave the origin immediately — the piece exists only along its path.
    board.detachPiece(piece);
    active_.emplace_back(piece->getId(), piece->getColor(), MotionPath(src, dst, nowMs));
    return true;
}

bool RealTimeArbiter::startJump(Board& board, const Position& cell, long long nowMs) {
    Piece* piece = board.getPieceAt(cell);
    if (piece == nullptr) return false;
    if (!JumpRules::canStart(*piece, isPieceMoving(piece->getId()))) return false;

    piece->setState(PieceState::Jumping);
    jumps_.push_back(Jump{ piece->getId(), cell, nowMs });
    return true;
}

std::optional<RealTimeArbiter::Occupant> RealTimeArbiter::findEarlierOccupant(
    int enteringPieceId, const Position& cell, long long timeMs, const Board& board) const {

    std::optional<Occupant> best;

    auto consider = [&](int pieceId, PieceColor color, long long sinceMs) {
        if (pieceId == enteringPieceId) {
            return;
        }
        if (sinceMs > timeMs) {
            return;
        }
        if (!best.has_value() || sinceMs < best->presentSinceMs ||
            (sinceMs == best->presentSinceMs && pieceId < best->pieceId)) {
            best = Occupant{ pieceId, color, sinceMs };
        }
    };

    for (const Motion& motion : active_) {
        if (!motion.path.occupies(cell, timeMs)) {
            continue;
        }

        long long since = motion.path.getStartMs();
        const auto& points = motion.path.waypoints();
        for (std::size_t i = 0; i < points.size(); ++i) {
            if (points[i] == cell) {
                since = motion.path.enterTime(i);
                break;
            }
        }
        consider(motion.pieceId, motion.color, since);
    }

    const Piece* stationary = board.getPieceAt(cell);
    if (stationary != nullptr && !isPieceMoving(stationary->getId())) {
        consider(stationary->getId(), stationary->getColor(),
                 std::numeric_limits<long long>::min() / 4);
    }

    return best;
}

void RealTimeArbiter::capturePiece(int pieceId, const Position& cell, Board& board, ArrivalReport& report) {
    Piece* piece = board.findPieceById(pieceId);
    if (piece == nullptr) {
        removeMotion(pieceId);
        removeJump(pieceId);
        return;
    }

    report.captures.push_back(CaptureEvent{
        piece->getId(),
        piece->getKind(),
        piece->getColor(),
        cell
    });

    removeMotion(pieceId);
    removeJump(pieceId);
    board.removePieceById(pieceId);
}

void RealTimeArbiter::landPiece(int pieceId, const Position& dest, Board& board, ArrivalReport& report) {
    Piece* piece = board.findPieceById(pieceId);
    removeMotion(pieceId);

    if (piece == nullptr) {
        return;
    }

    board.placePiece(piece, dest);
    piece->setState(PieceState::Idle);
    report.arrivals.push_back(ArrivalEvent{ pieceId, dest });
}

void RealTimeArbiter::resolveEntry(const EntryEvent& event, Board& board, ArrivalReport& report) {
    Motion* mover = findMotion(event.pieceId);
    if (mover == nullptr) {
        return;
    }

    const std::optional<Occupant> earlier =
        findEarlierOccupant(event.pieceId, event.cell, event.timeMs, board);

    if (!earlier.has_value()) {
        const bool isFinalWaypoint =
            event.waypointIndex + 1 >= mover->path.waypoints().size();
        if (isFinalWaypoint) {
            landPiece(event.pieceId, event.cell, board, report);
        }
        return;
    }

    const CollisionOutcome outcome =
        CollisionRules::resolve(mover->color, earlier->color);

    // Airborne defender: stays in cell and removes the arriving enemy.
    if (isPieceJumping(earlier->pieceId)) {
        if (outcome == CollisionOutcome::FriendlyBlock) {
            const Position stopCell = mover->path.previousCell(event.waypointIndex);
            landPiece(event.pieceId, stopCell, board, report);
            return;
        }
        capturePiece(event.pieceId, event.cell, board, report);
        return;
    }

    if (outcome == CollisionOutcome::FriendlyBlock) {
        const Position stopCell = mover->path.previousCell(event.waypointIndex);
        landPiece(event.pieceId, stopCell, board, report);
        return;
    }

    capturePiece(earlier->pieceId, event.cell, board, report);
    landPiece(event.pieceId, event.cell, board, report);
}

ArrivalReport RealTimeArbiter::advanceTo(long long nowMs, Board& board) {
    ArrivalReport report;

    enum class EventKind { Entry, JumpLand };

    struct TimedEvent {
        long long timeMs = 0;
        EventKind kind = EventKind::Entry;
        int pieceId = 0;
        std::size_t waypointIndex = 0;
        Position cell;
    };

    std::vector<TimedEvent> events;
    for (const Motion& motion : active_) {
        const auto& points = motion.path.waypoints();
        for (std::size_t i = 0; i < points.size(); ++i) {
            const long long t = motion.path.enterTime(i);
            if (t > lastAdvanceMs_ && t <= nowMs) {
                events.push_back(TimedEvent{ t, EventKind::Entry, motion.pieceId, i, points[i] });
            }
        }
    }
    for (const Jump& jump : jumps_) {
        const long long t = jump.landMs();
        if (t > lastAdvanceMs_ && t <= nowMs) {
            events.push_back(TimedEvent{ t, EventKind::JumpLand, jump.pieceId, 0, jump.cell });
        }
    }

    std::stable_sort(events.begin(), events.end(),
                     [](const TimedEvent& a, const TimedEvent& b) {
                         if (a.timeMs != b.timeMs) {
                             return a.timeMs < b.timeMs;
                         }
                         // At the exact land instant, resolve arrivals while still airborne.
                         if (a.kind != b.kind) {
                             return a.kind == EventKind::Entry;
                         }
                         return a.pieceId < b.pieceId;
                     });

    for (const TimedEvent& event : events) {
        if (event.kind == EventKind::Entry) {
            resolveEntry(EntryEvent{ event.timeMs, event.pieceId, event.waypointIndex, event.cell },
                         board, report);
        } else {
            landJump(event.pieceId, board);
        }
    }

    lastAdvanceMs_ = nowMs;
    return report;
}
