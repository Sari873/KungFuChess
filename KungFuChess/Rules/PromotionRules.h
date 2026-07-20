#pragma once
#include "../Model/Board.h"
#include "../Model/Piece.h"
#include "../Model/Position.h"
#include "PawnGeography.h"

class PromotionRules {
public:
    static bool shouldPromote(const Piece& piece, const Position& cell, const Board& board) {
        if (piece.getKind() != PieceKind::Pawn) return false;
        return cell.getRow() == PawnGeography::lastRow(piece.getColor(), board);
    }

    static PieceKind promotedKind() {
        return PieceKind::Queen;
    }
};
