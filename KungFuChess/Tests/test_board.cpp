#define TEST_SUITE_NAME "Board"
#include "TestHarness.h"
#include "TestHelpers.h"

TEST_CASE("keeps rows and cols distinct on a non-square board") {
    Board board(3, 5);
    check(board.getRows() == 3, "rows");
    check(board.getCols() == 5, "cols");
}

TEST_CASE("inBounds covers every edge") {
    Board board(3, 4);
    check(board.inBounds(Position(0, 0)), "top-left is inside");
    check(board.inBounds(Position(2, 3)), "bottom-right is inside");
    check(!board.inBounds(Position(3, 0)), "one row past the end is outside");
    check(!board.inBounds(Position(0, 4)), "one col past the end is outside");
    check(!board.inBounds(Position(-1, 0)), "a negative row is outside");
    check(!board.inBounds(Position(0, -1)), "a negative col is outside");
}

TEST_CASE("getPieceAt out of bounds returns null instead of crashing") {
    Board board(3, 3);
    check(board.getPieceAt(Position(99, 99)) == nullptr, "far out of bounds is null");
    check(board.getPieceAt(Position(-1, -1)) == nullptr, "negative is null");
}

TEST_CASE("addPiece occupies the cell") {
    Board board(3, 3);
    check(board.isEmpty(Position(1, 1)), "cell starts empty");
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    check(!board.isEmpty(Position(1, 1)), "cell is now occupied");
    check(board.getPieceAt(Position(1, 1))->getKind() == PieceKind::Rook, "the right kind landed");
    check(board.getPieceAt(Position(1, 1))->getColor() == PieceColor::White, "the right colour landed");
}

TEST_CASE("addPiece rejects an occupied cell") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 1, 1);
    bool ok = board.addPiece(std::make_unique<Piece>(500, PieceColor::Black, PieceKind::Pawn, Position(1, 1)));
    check(!ok, "cannot stack two pieces on one cell");
    check(board.getPieceAt(Position(1, 1))->getKind() == PieceKind::Rook, "the original piece is untouched");
}

TEST_CASE("addPiece rejects a cell off the board") {
    Board board(3, 3);
    bool ok = board.addPiece(std::make_unique<Piece>(501, PieceColor::Black, PieceKind::Pawn, Position(9, 9)));
    check(!ok, "cannot place a piece outside the board");
}

TEST_CASE("addPiece rejects a duplicate id") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    int existing = board.getPieceAt(Position(0, 0))->getId();
    bool ok = board.addPiece(std::make_unique<Piece>(existing, PieceColor::Black, PieceKind::Pawn, Position(2, 2)));
    check(!ok, "ids must stay unique");
    check(board.isEmpty(Position(2, 2)), "the rejected piece was not placed");
}

TEST_CASE("isFriendly compares colours") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 1);
    put(board, PieceColor::Black, PieceKind::Pawn, 0, 2);
    check(board.isFriendly(Position(0, 0), Position(0, 1)), "same colour is friendly");
    check(!board.isFriendly(Position(0, 0), Position(0, 2)), "opposite colour is not friendly");
}

TEST_CASE("an empty cell is never friendly") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    check(!board.isFriendly(Position(0, 0), Position(2, 2)), "empty destination is not friendly");
    check(!board.isFriendly(Position(2, 2), Position(0, 0)), "empty source is not friendly");
}

TEST_CASE("movePiece vacates the source and fills the destination") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    board.movePiece(Position(0, 0), Position(2, 0));
    check(board.isEmpty(Position(0, 0)), "the source is empty");
    check(board.getPieceAt(Position(2, 0))->getKind() == PieceKind::Rook, "the destination holds the rook");
    check(board.getPieceAt(Position(2, 0))->getCell() == Position(2, 0), "the piece knows where it now stands");
}

TEST_CASE("movePiece captures whatever stood on the destination") {
    Board board(3, 3);
    put(board, PieceColor::White, PieceKind::Rook, 0, 0);
    put(board, PieceColor::Black, PieceKind::King, 0, 2);
    int victimId = board.getPieceAt(Position(0, 2))->getId();

    board.movePiece(Position(0, 0), Position(0, 2));
    check(board.getPieceAt(Position(0, 2))->getId() != victimId, "the victim no longer holds the cell");
    check(board.getPieceAt(Position(0, 2))->getKind() == PieceKind::Rook, "the attacker holds it instead");
}

TEST_CASE("removePiece clears an occupied cell and refuses an empty one") {
    Board board(2, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 0);
    check(board.removePiece(Position(0, 0)), "removing an occupied cell succeeds");
    check(board.isEmpty(Position(0, 0)), "the cell is now empty");
    check(!board.removePiece(Position(0, 0)), "removing an empty cell fails");
}

TEST_CASE("a fresh piece is Idle") {
    Board board(2, 2);
    put(board, PieceColor::White, PieceKind::Pawn, 0, 0);
    check(board.getPieceAt(Position(0, 0))->getState() == PieceState::Idle, "state starts Idle");
}
