#define TEST_SUITE_NAME "KnightRules"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("makes all eight L jumps") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    check(legal(board, Position(2, 2), Position(0, 1)), "up-up-left");
    check(legal(board, Position(2, 2), Position(0, 3)), "up-up-right");
    check(legal(board, Position(2, 2), Position(1, 0)), "left-left-up");
    check(legal(board, Position(2, 2), Position(1, 4)), "right-right-up");
    check(legal(board, Position(2, 2), Position(3, 0)), "left-left-down");
    check(legal(board, Position(2, 2), Position(3, 4)), "right-right-down");
    check(legal(board, Position(2, 2), Position(4, 1)), "down-down-left");
    check(legal(board, Position(2, 2), Position(4, 3)), "down-down-right");
}

TEST_CASE("refuses every non-L shape") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    check(!legal(board, Position(2, 2), Position(2, 4)), "no straight line");
    check(!legal(board, Position(2, 2), Position(4, 4)), "no diagonal");
    check(!legal(board, Position(2, 2), Position(3, 3)), "no single diagonal step");
    check(!legal(board, Position(2, 2), Position(2, 3)), "no single sideways step");
    check(!legal(board, Position(2, 2), Position(4, 0)), "no long diagonal");
}

TEST_CASE("jumps straight out of a ring of eight blockers") {
    // This is the whole point of a knight, and the case a naive path-check breaks.
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 1, 1);
    put(board, PieceColor::White, PieceKind::Pawn, 1, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 1, 3);
    put(board, PieceColor::White, PieceKind::Pawn, 2, 1);
    put(board, PieceColor::White, PieceKind::Pawn, 2, 3);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 1);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 3);

    check(legal(board, Position(2, 2), Position(0, 1)), "escapes upward");
    check(legal(board, Position(2, 2), Position(4, 3)), "escapes downward");
    check(legal(board, Position(2, 2), Position(1, 0)), "escapes sideways");
}

TEST_CASE("blockers on the L path are irrelevant") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    put(board, PieceColor::Black, PieceKind::Queen, 3, 2); // directly on the way
    put(board, PieceColor::Black, PieceKind::Queen, 3, 3); // and on the diagonal
    check(legal(board, Position(2, 2), Position(4, 3)), "still jumps over both");
}

TEST_CASE("may not land on a friendly piece") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 4, 3);
    checkEq(why(board, Position(2, 2), Position(4, 3)), "friendly_destination", "no friendly landing");
}

TEST_CASE("captures an enemy at the end of the L") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Knight, 2, 2);
    put(board, PieceColor::Black, PieceKind::Pawn, 4, 3);
    check(legal(board, Position(2, 2), Position(4, 3)), "takes the enemy");
}

TEST_CASE("a cornered knight keeps only its on-board jumps") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Knight, 0, 0);
    check(legal(board, Position(0, 0), Position(1, 2)), "one legal jump survives");
    check(legal(board, Position(0, 0), Position(2, 1)), "and a second one");
    check(!legal(board, Position(0, 0), Position(2, 2)), "still refuses a non-L");
}
