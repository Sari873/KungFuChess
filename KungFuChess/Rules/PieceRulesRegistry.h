#pragma once
#include "PieceRules.h"
#include "BishopRules.h"
#include "KingRules.h"
#include "KnightRules.h"
#include "PawnRules.h"
#include "QueenRules.h"
#include "RookRules.h"
#include <array>


class PieceRulesRegistry {
public:
    static const PieceRulesRegistry& instance() {
        static const PieceRulesRegistry registry;
        return registry;
    }

    const PieceRules& get(PieceKind kind) const {
        return *table_[static_cast<std::size_t>(kind)];
    }

private:
    PieceRulesRegistry()
        : rook_(), bishop_(), queen_(), knight_(), king_(), pawn_() {
        table_[static_cast<std::size_t>(PieceKind::Rook)] = &rook_;
        table_[static_cast<std::size_t>(PieceKind::Bishop)] = &bishop_;
        table_[static_cast<std::size_t>(PieceKind::Queen)] = &queen_;
        table_[static_cast<std::size_t>(PieceKind::Knight)] = &knight_;
        table_[static_cast<std::size_t>(PieceKind::King)] = &king_;
        table_[static_cast<std::size_t>(PieceKind::Pawn)] = &pawn_;
    }

    RookRules rook_;
    BishopRules bishop_;
    QueenRules queen_;
    KnightRules knight_;
    KingRules king_;
    PawnRules pawn_;
    std::array<const PieceRules*, 6> table_{};
};
