#pragma once
#include "Position.h"
#include <string>

enum class PieceColor { White, Black };
enum class PieceKind { King, Queen, Rook, Bishop, Knight, Pawn };

// Piece.state is only a lifecycle flag - it does not store path,
// destination, elapsed time, speed, interpolation, or arrival logic.
// Those details belong to Motion / RealTimeArbiter in later iterations.
enum class PieceState { Idle, Moving, Captured };

// Piece never knows about the renderer, mouse clicks, pixels, or text test
// syntax.
class Piece {
public:
    Piece(int id, PieceColor color, PieceKind kind, Position cell);

    int getId() const;
    PieceColor getColor() const;
    PieceKind getKind() const;
    Position getCell() const;
    PieceState getState() const;

    void setCell(const Position& cell);
    void setState(PieceState state);

    // Two-letter board notation, e.g. "wK", "bR" - matches the text I/O
    // notation defined in the design guide (section 13).
    std::string toNotation() const;

    static PieceKind kindFromChar(char c);
    static char kindToChar(PieceKind kind);
    static PieceColor colorFromChar(char c);
    static char colorToChar(PieceColor color);

private:
    int id_;
    PieceColor color_;
    PieceKind kind_;
    Position cell_;
    PieceState state_;
};
