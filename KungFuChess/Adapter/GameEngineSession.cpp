#include "../GameConstants.h"
#include "GameEngineSession.h"
#include "../Model/Board.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"

namespace {

CellCoord toCell(const Position& pos) {
    return CellCoord(pos.getRow(), pos.getCol());
}

Position toPosition(const CellCoord& cell) {
    return Position(cell.row, cell.col);
}

DisplayColor toDisplayColor(PieceColor color) {
    return color == PieceColor::White ? DisplayColor::White : DisplayColor::Black;
}

DisplayKind toDisplayKind(PieceKind kind) {
    switch (kind) {
    case PieceKind::King: return DisplayKind::King;
    case PieceKind::Queen: return DisplayKind::Queen;
    case PieceKind::Rook: return DisplayKind::Rook;
    case PieceKind::Bishop: return DisplayKind::Bishop;
    case PieceKind::Knight: return DisplayKind::Knight;
    case PieceKind::Pawn: return DisplayKind::Pawn;
    }
    return DisplayKind::Pawn;
}

DisplayPieceState toDisplayState(PieceState state) {
    switch (state) {
    case PieceState::Idle: return DisplayPieceState::Idle;
    case PieceState::Moving: return DisplayPieceState::Moving;
    case PieceState::Jumping: return DisplayPieceState::Jumping;
    case PieceState::Captured: return DisplayPieceState::Captured;
    }
    return DisplayPieceState::Idle;
}

DisplayPiece toDisplayPiece(const Piece& piece) {
    DisplayPiece out;
    out.id = piece.getId();
    out.color = toDisplayColor(piece.getColor());
    out.kind = toDisplayKind(piece.getKind());
    out.cell = toCell(piece.getCell());
    out.state = toDisplayState(piece.getState());
    return out;
}

DisplayMotion toDisplayMotion(const MotionSnapshot& motion) {
    DisplayMotion out;
    out.pieceId = motion.pieceId;
    out.src = toCell(motion.src);
    out.dst = toCell(motion.dst);
    out.startMs = motion.startMs;
    out.durationMs = motion.arrivalMs - motion.startMs;
    if (out.durationMs < Kfc::Timing::kNonPositiveDeltaMs) {
        out.durationMs = Kfc::Timing::kNonPositiveDeltaMs;
    }
    return out;
}

DisplayJump toDisplayJump(const JumpSnapshot& jump) {
    const CellCoord at = toCell(jump.cell);
    DisplayJump out;
    out.pieceId = jump.pieceId;
    out.src = at;
    out.dst = at;
    out.startMs = jump.startMs;
    out.durationMs = jump.landMs - jump.startMs;
    if (out.durationMs < Kfc::Timing::kNonPositiveDeltaMs) {
        out.durationMs = Kfc::Timing::kNonPositiveDeltaMs;
    }
    return out;
}

DisplayBoard buildDisplayBoard(const Board& board) {
    const int rows = board.getRows();
    const int cols = board.getCols();
    std::vector<std::optional<DisplayPiece>> cells(static_cast<std::size_t>(rows * cols));

    std::vector<DisplayPiece> byId;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const Position pos(row, col);
            const Piece* piece = board.getPieceAt(pos);
            if (piece == nullptr) {
                continue;
            }
            DisplayPiece display = toDisplayPiece(*piece);
            cells[static_cast<std::size_t>(row * cols + col)] = display;
        }
    }

    for (int id = Kfc::Grid::kNeutral; id < Kfc::Limits::kMaxPieceIdScan; ++id) {
        const Piece* piece = board.findPieceById(id);
        if (piece == nullptr) {
            continue;
        }
        byId.push_back(toDisplayPiece(*piece));
    }

    return DisplayBoard(rows, cols, std::move(cells), std::move(byId));
}

} 

GameEngineSession::GameEngineSession(GameEngine& engine) : engine_(engine) {}

void GameEngineSession::advanceTime(long long deltaMs) {
    engine_.advanceTime(deltaMs);
}

long long GameEngineSession::clockMs() const {
    return engine_.getClockMs();
}

bool GameEngineSession::isGameOver() const {
    return engine_.isGameOver();
}

std::optional<DisplayColor> GameEngineSession::winner() const {
    const std::optional<PieceColor> w = engine_.winner();
    if (!w.has_value()) {
        return std::nullopt;
    }
    return toDisplayColor(*w);
}

DisplayBoard GameEngineSession::board() const {
    return buildDisplayBoard(engine_.getBoard());
}

bool GameEngineSession::isEmpty(const CellCoord& cell) const {
    return engine_.getBoard().isEmpty(toPosition(cell));
}

bool GameEngineSession::isMoving(const CellCoord& cell) const {
    return engine_.isMoving(toPosition(cell));
}

bool GameEngineSession::isJumping(const CellCoord& cell) const {
    return engine_.isJumping(toPosition(cell));
}

void GameEngineSession::requestMove(const CellCoord& src, const CellCoord& dst) {
    engine_.requestMove(toPosition(src), toPosition(dst));
}

void GameEngineSession::requestJump(const CellCoord& cell) {
    engine_.requestJump(toPosition(cell));
}

std::vector<CellCoord> GameEngineSession::legalDestinations(const CellCoord& src) const {
    const std::vector<Position> dests = engine_.legalDestinations(toPosition(src));
    std::vector<CellCoord> out;
    out.reserve(dests.size());
    for (const Position& pos : dests) {
        out.push_back(toCell(pos));
    }
    return out;
}

std::vector<DisplayMotion> GameEngineSession::activeMotions() const {
    const std::vector<MotionSnapshot> snapshots = engine_.activeMotions();
    std::vector<DisplayMotion> out;
    out.reserve(snapshots.size());
    for (const MotionSnapshot& motion : snapshots) {
        out.push_back(toDisplayMotion(motion));
    }
    return out;
}

std::vector<DisplayJump> GameEngineSession::activeJumps() const {
    const std::vector<JumpSnapshot> snapshots = engine_.activeJumps();
    std::vector<DisplayJump> out;
    out.reserve(snapshots.size());
    for (const JumpSnapshot& jump : snapshots) {
        out.push_back(toDisplayJump(jump));
    }
    return out;
}
