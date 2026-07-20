#pragma once
#include "Position.h"
#include <string>

enum class PieceColor { White, Black };
enum class PieceKind { King, Queen, Rook, Bishop, Knight, Pawn };
enum class PieceState { Idle, Moving, Jumping, Captured };

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
    void setKind(PieceKind kind);

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
