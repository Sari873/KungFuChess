#define TEST_SUITE_NAME "QueenRules"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("moves like a rook") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Queen, 2, 2);
    check(legal(board, Position(2, 2), Position(2, 0)), "left");
    check(legal(board, Position(2, 2), Position(2, 4)), "right");
    check(legal(board, Position(2, 2), Position(0, 2)), "up");
    check(legal(board, Position(2, 2), Position(4, 2)), "down");
}

TEST_CASE("moves like a bishop") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Queen, 2, 2);
    check(legal(board, Position(2, 2), Position(0, 0)), "up-left");
    check(legal(board, Position(2, 2), Position(0, 4)), "up-right");
    check(legal(board, Position(2, 2), Position(4, 0)), "down-left");
    check(legal(board, Position(2, 2), Position(4, 4)), "down-right");
}

TEST_CASE("still refuses the knight's L") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Queen, 2, 2);
    check(!legal(board, Position(2, 2), Position(4, 3)), "no L shape");
    check(!legal(board, Position(2, 2), Position(0, 1)), "no L shape the other way");
    check(!legal(board, Position(2, 2), Position(3, 0)), "no arbitrary jump");
}

TEST_CASE("is blocked on both straights and diagonals") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Queen, 0, 0);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 2, 2);
    checkEq(why(board, Position(0, 0), Position(0, 3)), "illegal_piece_move", "the straight is blocked");
    checkEq(why(board, Position(0, 0), Position(3, 3)), "illegal_piece_move", "the diagonal is blocked");
    check(legal(board, Position(0, 0), Position(4, 0)), "the free direction still works");
}

TEST_CASE("captures on both a straight and a diagonal") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Queen, 2, 2);
    put(board, PieceColor::Black, PieceKind::Pawn, 2, 4);
    put(board, PieceColor::Black, PieceKind::Pawn, 4, 4);
    check(legal(board, Position(2, 2), Position(2, 4)), "takes along the rank");
    check(legal(board, Position(2, 2), Position(4, 4)), "takes along the diagonal");
}
