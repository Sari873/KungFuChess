#define TEST_SUITE_NAME "GameEngine"
#include "TestHarness.h"
#include "TestHelpers.h"
#include "../Engine/GameEngine.h"
#include "../Model/GameState.h"

TEST_CASE("a new game starts at zero and is not over") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};
    checkEq(engine.getClockMs(), 0, "the clock starts at zero");
    check(!engine.isGameOver(), "the game is not over");
}

TEST_CASE("rule-level reasons pass straight through") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 1);
    GameEngine engine{GameState(std::move(board))};

    checkEq(engine.requestMove(Position(0, 0), Position(1, 1)).reason, "illegal_piece_move", "bad shape");
    checkEq(engine.requestMove(Position(2, 2), Position(2, 1)).reason, "empty_source", "empty source");
    checkEq(engine.requestMove(Position(0, 0), Position(9, 9)).reason, "outside_board", "off the board");
    checkEq(engine.requestMove(Position(0, 0), Position(0, 1)).reason, "friendly_destination", "own piece");
}

TEST_CASE("a rejected command launches nothing") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(1, 1)); // illegal diagonal
    engine.advanceTime(99999);
    check(!engine.getBoard().isEmpty(Position(0, 0)), "the rook never left");
}

TEST_CASE("an accepted command does not move the piece yet") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    MoveResult result = engine.requestMove(Position(0, 0), Position(0, 2));
    check(result.is_accepted, "accepted");
    checkEq(result.reason, "ok", "reason is ok");
    check(!engine.getBoard().isEmpty(Position(0, 0)), "the board has not changed");
    check(engine.isMoving(Position(0, 0)), "the piece is in flight");
}

TEST_CASE("a piece in flight is rejected with motion_in_progress") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 2));
    checkEq(engine.requestMove(Position(0, 0), Position(1, 0)).reason, "motion_in_progress",
            "a second command mid-flight is refused");
}

TEST_CASE("the move becomes visible only after enough time") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 2)); // 2 cells -> 2000ms

    engine.advanceTime(1999);
    check(!engine.getBoard().isEmpty(Position(0, 0)), "not yet at t=1999");

    engine.advanceTime(1);
    check(engine.getBoard().isEmpty(Position(0, 0)), "the source is vacated at t=2000");
    check(!engine.getBoard().isEmpty(Position(0, 2)), "the destination is filled at t=2000");
}

TEST_CASE("capturing the king ends the game") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 1);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 1));
    check(!engine.isGameOver(), "the game is not over while the move is in flight");

    engine.advanceTime(1000);
    check(engine.isGameOver(), "it ends when the capture actually lands");
}

TEST_CASE("capturing a non-king does not end the game") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Pawn, 0, 1);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 1));
    engine.advanceTime(1000);
    check(!engine.getBoard().isEmpty(Position(0, 1)), "the pawn was taken");
    check(!engine.isGameOver(), "but the game goes on");
}

TEST_CASE("game_over outranks every other guard") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 1);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 1));
    engine.advanceTime(1000);

    // The knight's move is perfectly legal - and still refused.
    checkEq(engine.requestMove(Position(2, 2), Position(1, 0)).reason, "game_over",
            "a legal move is refused once the game is over");

    engine.advanceTime(99999);
    check(!engine.getBoard().isEmpty(Position(2, 2)), "the board is frozen");
}

TEST_CASE("advanceTime ignores zero and negative deltas") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.requestMove(Position(0, 0), Position(0, 1));
    engine.advanceTime(0);
    engine.advanceTime(-9999);

    checkEq(engine.getClockMs(), 0, "the clock did not move");
    check(!engine.getBoard().isEmpty(Position(0, 0)), "time never ran backwards into an arrival");
}

TEST_CASE("the clock accumulates across calls") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.advanceTime(300);
    engine.advanceTime(700);
    checkEq(engine.getClockMs(), 1000, "300 + 700 = 1000");
}

TEST_CASE("a motion started later lands later") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    GameEngine engine{GameState(std::move(board))};

    engine.advanceTime(5000);                            // time passes first
    engine.requestMove(Position(0, 0), Position(0, 1));  // launched at t=5000

    engine.advanceTime(999);
    check(!engine.getBoard().isEmpty(Position(0, 0)), "still flying at t=5999");
    engine.advanceTime(1);
    check(!engine.getBoard().isEmpty(Position(0, 1)), "lands at t=6000, not at t=1000");
}
