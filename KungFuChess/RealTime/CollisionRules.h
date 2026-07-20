#pragma once
#include "../Model/Piece.h"

enum class CollisionOutcome {
    None,
    FriendlyBlock,   
    HostileCapture   
};

class CollisionRules {
public:
    static CollisionOutcome resolve(PieceColor later, PieceColor earlier) {
        if (later == earlier) {
            return CollisionOutcome::FriendlyBlock;
        }
        return CollisionOutcome::HostileCapture;
    }
};
