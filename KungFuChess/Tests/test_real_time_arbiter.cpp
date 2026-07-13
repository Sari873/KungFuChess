#define TEST_SUITE_NAME "RealTimeArbiter"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../RealTime/RealTimeArbiter.h"

TEST_CASE("starting a motion does NOT move the piece") {
    // The heart of the real-time iteration.
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;

    check(arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0), "the motion is accepted");
    check(!board.isEmpty(Position(0, 0)), "the piece is STILL on its source cell");
    check(board.isEmpty(Position(0, 1)), "the destination is still empty");
    check(arbiter.hasActiveMotions(), "the motion is registered");
}

TEST_CASE("a launched piece is flagged Moving") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    check(board.getPieceAt(Position(0, 0))->getState() == PieceState::Moving, "state is Moving");
    check(arbiter.isMoving(board, Position(0, 0)), "the arbiter agrees");
}

TEST_CASE("a piece in flight refuses a second motion") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    check(!arbiter.startMotion(board, Position(0, 0), Position(0, 2), 0), "no second command mid-flight");
}

TEST_CASE("nothing lands before the arrival time") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    arbiter.advanceTo(999, board);
    check(!board.isEmpty(Position(0, 0)), "at t=999 the piece has not moved");
    check(arbiter.hasActiveMotions(), "the motion is still in flight");
}

TEST_CASE("the piece lands exactly on time and returns to Idle") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    arbiter.advanceTo(1000, board);
    check(board.isEmpty(Position(0, 0)), "the source is vacated");
    check(!board.isEmpty(Position(0, 1)), "the destination is filled");
    check(board.getPieceAt(Position(0, 1))->getState() == PieceState::Idle, "the piece is Idle again");
    check(!arbiter.hasActiveMotions(), "the motion is dropped");
}

TEST_CASE("arriving on an enemy reports a capture event") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 1);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    auto early = arbiter.advanceTo(500, board);
    check(early.empty(), "no event before arrival");

    auto events = arbiter.advanceTo(1000, board);
    check(events.size() == 1, "exactly one event");
    check(events[0].capturedKind == PieceKind::King, "the event names the kind");
    check(events[0].capturedColor == PieceColor::Black, "the event names the colour");
    check(events[0].cell == Position(0, 1), "the event names the cell");
}

TEST_CASE("arriving on an empty cell reports nothing") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);

    auto events = arbiter.advanceTo(1000, board);
    check(events.empty(), "a quiet arrival raises no capture event");
}

TEST_CASE("two pieces fly at once") {
    // Simultaneous movement is the point of Kung Fu Chess.
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 0);
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);
    arbiter.startMotion(board, Position(2, 0), Position(2, 1), 0);
    check(arbiter.hasActiveMotions(), "both are in flight");

    arbiter.advanceTo(1000, board);
    check(!board.isEmpty(Position(0, 1)), "the first landed");
    check(!board.isEmpty(Position(2, 1)), "the second landed");
    check(!arbiter.hasActiveMotions(), "both are cleared");
}

TEST_CASE("a longer move lands later than a shorter one") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);  // 2 cells -> 2000ms
    put(board, PieceColor::Black, PieceKind::Rook, 2, 0);  // 1 cell  -> 1000ms
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 2), 0);
    arbiter.startMotion(board, Position(2, 0), Position(2, 1), 0);

    arbiter.advanceTo(1000, board);
    check(!board.isEmpty(Position(2, 1)), "the short move has landed");
    check(!board.isEmpty(Position(0, 0)), "the long move has NOT landed");
    check(arbiter.hasActiveMotions(), "the long move is still flying");

    arbiter.advanceTo(2000, board);
    check(!board.isEmpty(Position(0, 2)), "the long move lands on its own schedule");
}

TEST_CASE("one large advance resolves every pending arrival") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 0);
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);
    arbiter.startMotion(board, Position(2, 0), Position(2, 1), 0);
    arbiter.advanceTo(999999, board);

    check(!board.isEmpty(Position(0, 1)) && !board.isEmpty(Position(2, 1)), "both landed in one jump");
    check(!arbiter.hasActiveMotions(), "nothing is left in flight");
}

TEST_CASE("two pieces racing to one cell do not stack") {
    // The rook (1 cell, lands at 1000) beats the knight (L jump, lands at 2000).
    // The knight's move must be abandoned, not stacked on top.
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    RealTimeArbiter arbiter;

    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);
    arbiter.startMotion(board, Position(2, 2), Position(0, 1), 0);
    arbiter.advanceTo(2000, board);

    check(board.getPieceAt(Position(0, 1))->getKind() == PieceKind::Rook, "the winner holds the cell");
    check(!board.isEmpty(Position(2, 2)), "the loser stayed home");
    check(board.getPieceAt(Position(2, 2))->getState() == PieceState::Idle,
          "the loser is Idle again, not stuck as Moving forever");
}

TEST_CASE("a motion from an empty cell is refused") {
    Board board(3, 3);
    RealTimeArbiter arbiter;
    check(!arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0), "nothing to launch");
    check(!arbiter.hasActiveMotions(), "no motion was registered");
}

TEST_CASE("isMoving on an empty cell is false, not a crash") {
    Board board(3, 3);
    RealTimeArbiter arbiter;
    check(!arbiter.isMoving(board, Position(1, 1)), "an empty cell is not moving");
}
