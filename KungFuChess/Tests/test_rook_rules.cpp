#define TEST_SUITE_NAME "RookRules"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("slides in all four straight directions") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Rook, 2, 2);
    check(legal(board, Position(2, 2), Position(2, 4)), "right");
    check(legal(board, Position(2, 2), Position(2, 0)), "left");
    check(legal(board, Position(2, 2), Position(0, 2)), "up");
    check(legal(board, Position(2, 2), Position(4, 2)), "down");
}

TEST_CASE("refuses diagonals and L shapes") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Rook, 2, 2);
    check(!legal(board, Position(2, 2), Position(4, 4)), "no diagonal");
    check(!legal(board, Position(2, 2), Position(0, 0)), "no diagonal the other way");
    check(!legal(board, Position(2, 2), Position(3, 4)), "no L shape");
}

TEST_CASE("stops short of a friendly blocker and cannot pass it") {
    Board board(1, 5);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 2);
    check(legal(board, Position(0, 0), Position(0, 1)), "may stop before the blocker");
    checkEq(why(board, Position(0, 0), Position(0, 2)), "friendly_destination", "may not land on it");
    checkEq(why(board, Position(0, 0), Position(0, 3)), "illegal_piece_move", "may not jump it");
    checkEq(why(board, Position(0, 0), Position(0, 4)), "illegal_piece_move", "may not reach far past it");
}

TEST_CASE("captures the first enemy in its path but cannot pass it") {
    Board board(1, 5);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Pawn, 0, 2);
    check(legal(board, Position(0, 0), Position(0, 2)), "may take the enemy");
    checkEq(why(board, Position(0, 0), Position(0, 3)), "illegal_piece_move", "may not pass through it");
}

TEST_CASE("captures an adjacent enemy") {
    Board board(1, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Pawn, 0, 1);
    check(legal(board, Position(0, 0), Position(0, 1)), "the very next cell can be taken");
}

TEST_CASE("is blocked in one direction but free in another") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Rook, 2, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 2, 3);
    checkEq(why(board, Position(2, 2), Position(2, 4)), "illegal_piece_move", "right is blocked");
    check(legal(board, Position(2, 2), Position(2, 0)), "left is still open");
    check(legal(board, Position(2, 2), Position(0, 2)), "up is still open");
}

TEST_CASE("travels the full length of an open board") {
    Board board(8, 8);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    check(legal(board, Position(0, 0), Position(0, 7)), "crosses seven cells");
    check(legal(board, Position(0, 0), Position(7, 0)), "and seven rows");
}
