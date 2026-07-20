#pragma once
#include "../Model/Piece.h"
#include "Jump.h"

/** Eligibility and timing policy for the Jump ability. */
class JumpRules {
public:
    static constexpr long long DURATION_MS = Jump::DURATION_MS;

    static bool canStart(const Piece& piece, bool isCurrentlyMoving) {
        if (isCurrentlyMoving) {
            return false;
        }
        switch (piece.getState()) {
        case PieceState::Idle:
            return true;
        case PieceState::Moving:
        case PieceState::Jumping:
        case PieceState::Captured:
            return false;
        }
        return false;
    }
};
