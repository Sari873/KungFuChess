#define TEST_SUITE_NAME "RuleEngine"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("rejects a destination off the board") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    checkEq(why(board, Position(1, 1), Position(9, 9)), "outside_board", "far off the board");
    checkEq(why(board, Position(1, 1), Position(-1, 1)), "outside_board", "negative row");
    checkEq(why(board, Position(1, 1), Position(1, -1)), "outside_board", "negative col");
}

TEST_CASE("rejects a source off the board") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    checkEq(why(board, Position(9, 9), Position(1, 1)), "outside_board", "source is off the board");
}

TEST_CASE("rejects an empty source cell") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    checkEq(why(board, Position(0, 0), Position(0, 1)), "empty_source", "nothing to move");
}

TEST_CASE("rejects landing on your own piece") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    put(board, PieceColor::White, PieceKind::Pawn, 1, 2);
    checkEq(why(board, Position(1, 1), Position(1, 2)), "friendly_destination", "no friendly fire");
}

TEST_CASE("allows capturing an enemy") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    put(board, PieceColor::Black, PieceKind::Pawn, 1, 2);
    checkEq(why(board, Position(1, 1), Position(1, 2)), "ok", "an enemy may be taken");
}

TEST_CASE("rejects a shape the piece cannot make") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    checkEq(why(board, Position(1, 1), Position(2, 2)), "illegal_piece_move", "rooks do not go diagonally");
}

TEST_CASE("checks bounds before shape") {
    // An off-board target is reported as outside_board even though the shape
    // would also have been illegal. Guard order matters for stable reasons.
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    checkEq(why(board, Position(1, 1), Position(5, 7)), "outside_board", "bounds win over shape");
}

TEST_CASE("checks the empty source before the shape") {
    Board board(3, 3);
    checkEq(why(board, Position(0, 0), Position(2, 2)), "empty_source", "empty source wins over shape");
}

TEST_CASE("a piece may not move to its own cell") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Queen, 1, 1);
    check(!legal(board, Position(1, 1), Position(1, 1)), "staying put is not a move");
}

TEST_CASE("validation never mutates the board") {
    Board board(1, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::Pawn, 0, 2);

    RuleEngine::validateMove(board, Position(0, 0), Position(0, 2));

    check(!board.isEmpty(Position(0, 0)), "the source is still occupied");
    check(board.getPieceAt(Position(0, 2))->getKind() == PieceKind::Pawn, "the capture was NOT executed");
}
