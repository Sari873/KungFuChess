#define TEST_SUITE_NAME "KingRules"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("steps one cell in all eight directions") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::King, 2, 2);
    check(legal(board, Position(2, 2), Position(1, 2)), "up");
    check(legal(board, Position(2, 2), Position(3, 2)), "down");
    check(legal(board, Position(2, 2), Position(2, 1)), "left");
    check(legal(board, Position(2, 2), Position(2, 3)), "right");
    check(legal(board, Position(2, 2), Position(1, 1)), "up-left");
    check(legal(board, Position(2, 2), Position(1, 3)), "up-right");
    check(legal(board, Position(2, 2), Position(3, 1)), "down-left");
    check(legal(board, Position(2, 2), Position(3, 3)), "down-right");
}

TEST_CASE("refuses two cells in any direction") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::King, 2, 2);
    checkEq(why(board, Position(2, 2), Position(2, 4)), "illegal_piece_move", "two sideways");
    checkEq(why(board, Position(2, 2), Position(4, 2)), "illegal_piece_move", "two vertically");
    checkEq(why(board, Position(2, 2), Position(4, 4)), "illegal_piece_move", "two diagonally");
    checkEq(why(board, Position(2, 2), Position(0, 0)), "illegal_piece_move", "two diagonally the other way");
}

TEST_CASE("refuses a knight's L") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::King, 2, 2);
    check(!legal(board, Position(2, 2), Position(4, 3)), "no L shape");
}

TEST_CASE("may not step onto its own piece") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::King, 1, 1);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 1);
    checkEq(why(board, Position(1, 1), Position(0, 1)), "friendly_destination", "no friendly fire");
}

TEST_CASE("captures an adjacent enemy") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::King, 1, 1);
    put(board, PieceColor::Black, PieceKind::Pawn, 1, 0);
    check(legal(board, Position(1, 1), Position(1, 0)), "takes the neighbour");
}

TEST_CASE("a cornered king can still step inward") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::King, 0, 0);
    check(legal(board, Position(0, 0), Position(0, 1)), "steps right");
    check(legal(board, Position(0, 0), Position(1, 0)), "steps down");
    check(legal(board, Position(0, 0), Position(1, 1)), "steps diagonally inward");
    check(!legal(board, Position(0, 0), Position(0, 2)), "but never leaps");
}
