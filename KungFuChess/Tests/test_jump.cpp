#define TEST_SUITE_NAME "Jump"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../Engine/GameEngine.h"
#include "../Model/GameState.h"
#include "../RealTime/Jump.h"
#include "../RealTime/RealTimeArbiter.h"

TEST_CASE("a jump lasts exactly 1000ms and keeps the piece on its cell") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    const int id = board.getPieceAt(Position(0, 0))->getId();
    RealTimeArbiter arbiter;

    check(arbiter.startJump(board, Position(0, 0), 0), "jump starts");
    check(!board.isEmpty(Position(0, 0)), "piece stays on its cell");
    check(board.getPieceAt(Position(0, 0))->getState() == PieceState::Jumping, "state is Jumping");
    check(arbiter.isPieceJumping(id), "arbiter tracks the jump");
    check(arbiter.hasActiveJumps(), "jump is active");

    arbiter.advanceTo(999, board);
    check(board.getPieceAt(Position(0, 0))->getState() == PieceState::Jumping, "still airborne at 999");

    arbiter.advanceTo(1000, board);
    check(board.getPieceAt(Position(0, 0))->getState() == PieceState::Idle, "lands at 1000");
    check(!arbiter.hasActiveJumps(), "jump cleared");
    check(!board.isEmpty(Position(0, 0)), "still on the same cell");
}

TEST_CASE("moving and captured pieces cannot jump") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Pawn, 2, 2);
    RealTimeArbiter arbiter;

    board.getPieceAt(Position(0, 0))->setState(PieceState::Moving);
    check(!arbiter.startJump(board, Position(0, 0), 0), "Moving state cannot jump");

    board.getPieceAt(Position(2, 2))->setState(PieceState::Captured);
    check(!arbiter.startJump(board, Position(2, 2), 0), "Captured state cannot jump");
}

TEST_CASE("an airborne piece captures an enemy that enters its cell") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 1); // jumper
    put(board, PieceColor::Black, PieceKind::Rook, 0, 0); // arrives on jumper's cell
    const int whiteId = board.getPieceAt(Position(0, 1))->getId();
    const int blackId = board.getPieceAt(Position(0, 0))->getId();

    RealTimeArbiter arbiter;
    arbiter.startMotion(board, Position(0, 0), Position(0, 1), 0);   // lands at t=1000
    arbiter.startJump(board, Position(0, 1), 500);                   // airborne until t=1500

    auto report = arbiter.advanceTo(1000, board);
    check(report.captures.size() == 1, "one capture");
    check(report.captures[0].capturedPieceId == blackId, "the arriving enemy is removed");
    check(report.captures[0].cell == Position(0, 1), "capture is on the jump cell");
    check(board.findPieceById(blackId) == nullptr, "enemy is gone");
    check(board.getPieceAt(Position(0, 1))->getId() == whiteId, "jumper remains on cell");
    check(board.getPieceAt(Position(0, 1))->getState() == PieceState::Jumping,
          "jumper is still airborne after the mid-air capture");

    arbiter.advanceTo(1500, board);
    check(board.getPieceAt(Position(0, 1))->getState() == PieceState::Idle, "then lands normally");
}

TEST_CASE("after a quiet jump window the piece lands idle") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Knight, 1, 1);
    GameEngine engine{ GameState(std::move(board)) };

    check(engine.requestJump(Position(1, 1)).is_accepted, "jump accepted");
    check(engine.isJumping(Position(1, 1)), "engine reports jumping");
    checkEq(engine.requestMove(Position(1, 1), Position(2, 3)).reason, "jump_in_progress",
            "cannot move while jumping");

    engine.advanceTime(1000);
    check(!engine.isJumping(Position(1, 1)), "no longer jumping");
    check(engine.getBoard().getPieceAt(Position(1, 1))->getState() == PieceState::Idle, "idle again");
}

TEST_CASE("engine rejects jump for a moving piece") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{ GameState(std::move(board)) };

    engine.requestMove(Position(0, 0), Position(0, 2));
    checkEq(engine.requestJump(Position(0, 0)).reason, "empty_source",
            "origin is vacated while moving");
}

TEST_CASE("jump duration constant is 1000ms") {
    checkEq(Jump::DURATION_MS, 1000LL, "jump window");
}
