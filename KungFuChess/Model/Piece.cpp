#include "Piece.h"
#include <stdexcept>

Piece::Piece(int id, PieceColor color, PieceKind kind, Position cell)
    : id_(id), color_(color), kind_(kind), cell_(cell), state_(PieceState::Idle) {}

int Piece::getId() const { return id_; }
PieceColor Piece::getColor() const { return color_; }
PieceKind Piece::getKind() const { return kind_; }
Position Piece::getCell() const { return cell_; }
PieceState Piece::getState() const { return state_; }

void Piece::setCell(const Position& cell) { cell_ = cell; }
void Piece::setState(PieceState state) { state_ = state; }

std::string Piece::toNotation() const {
    std::string result;
    result += colorToChar(color_);
    result += kindToChar(kind_);
    return result;
}

PieceKind Piece::kindFromChar(char c) {
    switch (c) {
        case 'K': return PieceKind::King;
        case 'Q': return PieceKind::Queen;
        case 'R': return PieceKind::Rook;
        case 'B': return PieceKind::Bishop;
        case 'N': return PieceKind::Knight;
        case 'P': return PieceKind::Pawn;
        default: throw std::invalid_argument("Unknown piece letter");
    }
}

char Piece::kindToChar(PieceKind kind) {
    switch (kind) {
        case PieceKind::King:   return 'K';
        case PieceKind::Queen:  return 'Q';
        case PieceKind::Rook:   return 'R';
        case PieceKind::Bishop: return 'B';
        case PieceKind::Knight: return 'N';
        case PieceKind::Pawn:   return 'P';
    }
    return '?'; // unreachable
}

PieceColor Piece::colorFromChar(char c) {
    if (c == 'w') return PieceColor::White;
    if (c == 'b') return PieceColor::Black;
    throw std::invalid_argument("Unknown color letter");
}

char Piece::colorToChar(PieceColor color) {
    return color == PieceColor::White ? 'w' : 'b';
}
