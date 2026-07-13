#define TEST_SUITE_NAME "BishopRules"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("slides on all four diagonals") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Bishop, 2, 2);
    check(legal(board, Position(2, 2), Position(0, 0)), "up-left");
    check(legal(board, Position(2, 2), Position(0, 4)), "up-right");
    check(legal(board, Position(2, 2), Position(4, 0)), "down-left");
    check(legal(board, Position(2, 2), Position(4, 4)), "down-right");
}

TEST_CASE("refuses straight lines") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Bishop, 2, 2);
    check(!legal(board, Position(2, 2), Position(2, 4)), "no horizontal");
    check(!legal(board, Position(2, 2), Position(0, 2)), "no vertical");
    check(!legal(board, Position(2, 2), Position(3, 4)), "no L shape");
}

TEST_CASE("stops short of a diagonal blocker and cannot pass it") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Bishop, 0, 0);
    put(board, PieceColor::White, PieceKind::Pawn, 2, 2);
    check(legal(board, Position(0, 0), Position(1, 1)), "may stop before it");
    checkEq(why(board, Position(0, 0), Position(2, 2)), "friendly_destination", "may not land on it");
    checkEq(why(board, Position(0, 0), Position(3, 3)), "illegal_piece_move", "may not jump it");
}

TEST_CASE("captures the first enemy on the diagonal but cannot pass it") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Bishop, 0, 0);
    put(board, PieceColor::Black, PieceKind::Pawn, 2, 2);
    check(legal(board, Position(0, 0), Position(2, 2)), "may take it");
    checkEq(why(board, Position(0, 0), Position(3, 3)), "illegal_piece_move", "may not pass through it");
}

TEST_CASE("a blocker on one diagonal does not block another") {
    Board board(5, 5);
    put(board, PieceColor::White, PieceKind::Bishop, 2, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 1, 1);
    checkEq(why(board, Position(2, 2), Position(0, 0)), "illegal_piece_move", "up-left is blocked");
    check(legal(board, Position(2, 2), Position(4, 4)), "down-right is still open");
    check(legal(board, Position(2, 2), Position(0, 4)), "up-right is still open");
}
