#define TEST_SUITE_NAME "PawnRules"
#include "TestHarness.h"
#include "TestHelpers.h"

// Orientation: White advances toward row 0, Black toward higher rows.

TEST_CASE("white advances up and black advances down") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::Black, PieceKind::Pawn, 1, 2);
    check(legal(board, Position(3, 2), Position(2, 2)), "white moves up one");
    check(legal(board, Position(1, 2), Position(2, 2)), "black moves down one");
}

TEST_CASE("neither colour may move backwards") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::Black, PieceKind::Pawn, 1, 2);
    check(!legal(board, Position(3, 2), Position(4, 2)), "white cannot retreat");
    check(!legal(board, Position(1, 2), Position(0, 2)), "black cannot retreat");
}

TEST_CASE("may not advance two cells") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 4, 2);
    put(board, PieceColor::Black, PieceKind::Pawn, 0, 0);
    checkEq(why(board, Position(4, 2), Position(2, 2)), "illegal_piece_move", "white cannot double-step");
    checkEq(why(board, Position(0, 0), Position(2, 0)), "illegal_piece_move", "black cannot double-step");
}

TEST_CASE("may not move sideways") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    check(!legal(board, Position(3, 2), Position(3, 3)), "no sideways step");
    check(!legal(board, Position(3, 2), Position(3, 1)), "no sideways step the other way");
}

TEST_CASE("captures diagonally forward") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 1);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 3);
    check(legal(board, Position(3, 2), Position(2, 1)), "white takes up-left");
    check(legal(board, Position(3, 2), Position(2, 3)), "white takes up-right");
}

TEST_CASE("black captures diagonally downward") {
    Board board(5, 5);
    put(board, PieceColor::Black, PieceKind::Pawn, 1, 2);
    put(board, PieceColor::White, PieceKind::Rook, 2, 1);
    put(board, PieceColor::White, PieceKind::Rook, 2, 3);
    check(legal(board, Position(1, 2), Position(2, 1)), "black takes down-left");
    check(legal(board, Position(1, 2), Position(2, 3)), "black takes down-right");
}

TEST_CASE("may not capture diagonally backwards") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 2, 2);
    put(board, PieceColor::Black, PieceKind::Rook, 3, 1);
    check(!legal(board, Position(2, 2), Position(3, 1)), "white cannot take behind itself");
}

TEST_CASE("may not step diagonally onto an empty cell") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    check(!legal(board, Position(3, 2), Position(2, 1)), "the diagonal needs a victim");
    check(!legal(board, Position(3, 2), Position(2, 3)), "on both sides");
}

TEST_CASE("may NOT capture straight ahead") {
    // The asymmetry that defines a pawn: forward is movement only, never capture.
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::Black, PieceKind::Rook, 2, 2);
    checkEq(why(board, Position(3, 2), Position(2, 2)), "illegal_piece_move", "an enemy ahead blocks, it is not taken");
}

TEST_CASE("is blocked by its own piece ahead") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::White, PieceKind::Rook, 2, 2);
    checkEq(why(board, Position(3, 2), Position(2, 2)), "friendly_destination", "a friend ahead blocks too");
}

TEST_CASE("may not capture its own piece diagonally") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Pawn, 3, 2);
    put(board, PieceColor::White, PieceKind::Rook, 2, 1);
    checkEq(why(board, Position(3, 2), Position(2, 1)), "friendly_destination", "no friendly fire on the diagonal");
}

TEST_CASE("a pawn on the last rank has nowhere to go") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 1);
    check(!legal(board, Position(0, 1), Position(1, 1)), "cannot go backwards off the edge case");
}
