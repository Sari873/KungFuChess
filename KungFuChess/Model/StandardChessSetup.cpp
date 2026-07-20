#include "StandardChessSetup.h"
#include <memory>

namespace {

void placeBackRank(Board& board, PieceColor color, int row, int& nextId) {
    const PieceKind order[] = {
        PieceKind::Rook, PieceKind::Knight, PieceKind::Bishop, PieceKind::Queen,
        PieceKind::King, PieceKind::Bishop, PieceKind::Knight, PieceKind::Rook
    };
    for (int col = 0; col < 8; ++col) {
        board.addPiece(std::make_unique<Piece>(nextId++, color, order[col], Position(row, col)));
    }
}

void placePawns(Board& board, PieceColor color, int row, int& nextId) {
    for (int col = 0; col < 8; ++col) {
        board.addPiece(std::make_unique<Piece>(nextId++, color, PieceKind::Pawn, Position(row, col)));
    }
}

} 

Board StandardChessSetup::create() {
    Board board(8, 8);
    int nextId = 1;

    placeBackRank(board, PieceColor::Black, 0, nextId);
    placePawns(board, PieceColor::Black, 1, nextId);
    placePawns(board, PieceColor::White, 6, nextId);
    placeBackRank(board, PieceColor::White, 7, nextId);

    return board;
}
