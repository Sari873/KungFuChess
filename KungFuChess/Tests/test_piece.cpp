#define TEST_SUITE_NAME "Piece"
#include "TestHarness.h"
#include "../Model/Piece.h"

TEST_CASE("round-trips board notation") {
    Piece whiteKing(1, PieceColor::White, PieceKind::King, Position(0, 0));
    Piece blackKnight(2, PieceColor::Black, PieceKind::Knight, Position(1, 1));
    checkEq(whiteKing.toNotation(), "wK", "white king prints as wK");
    checkEq(blackKnight.toNotation(), "bN", "black knight prints as bN");
}

TEST_CASE("every kind has a letter, and it survives a round trip") {
    const PieceKind kinds[6] = {
        PieceKind::King, PieceKind::Queen, PieceKind::Rook,
        PieceKind::Bishop, PieceKind::Knight, PieceKind::Pawn
    };
    for (PieceKind kind : kinds) {
        char letter = Piece::kindToChar(kind);
        check(Piece::kindFromChar(letter) == kind, "kind survives char round trip");
    }
}

TEST_CASE("the knight is N, not K - the classic collision") {
    check(Piece::kindToChar(PieceKind::Knight) == 'N', "knight is N");
    check(Piece::kindToChar(PieceKind::King) == 'K', "king is K");
    check(Piece::kindFromChar('N') == PieceKind::Knight, "N parses as knight");
    check(Piece::kindFromChar('K') == PieceKind::King, "K parses as king");
}

TEST_CASE("colours round-trip") {
    check(Piece::colorFromChar('w') == PieceColor::White, "w is white");
    check(Piece::colorFromChar('b') == PieceColor::Black, "b is black");
    check(Piece::colorToChar(PieceColor::White) == 'w', "white is w");
    check(Piece::colorToChar(PieceColor::Black) == 'b', "black is b");
}

TEST_CASE("state is a plain lifecycle flag we can set") {
    Piece piece(1, PieceColor::White, PieceKind::Rook, Position(0, 0));
    check(piece.getState() == PieceState::Idle, "starts Idle");
    piece.setState(PieceState::Moving);
    check(piece.getState() == PieceState::Moving, "can become Moving");
    piece.setState(PieceState::Jumping);
    check(piece.getState() == PieceState::Jumping, "can become Jumping");
    piece.setState(PieceState::Captured);
    check(piece.getState() == PieceState::Captured, "can become Captured");
}
