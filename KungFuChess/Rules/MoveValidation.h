#pragma once
#include <string>

// Result shape for RuleEngine, per the design guide section 8.
// 'reason' is always present: "ok" for a valid move, or a stable
// machine-readable string for an invalid one - "outside_board",
// "empty_source", "friendly_destination", "illegal_piece_move".
struct MoveValidation {
    bool is_valid;
    std::string reason;

    static MoveValidation ok() {
        return MoveValidation{true, "ok"};
    }

    static MoveValidation fail(const std::string& reason) {
        return MoveValidation{false, reason};
    }
};
