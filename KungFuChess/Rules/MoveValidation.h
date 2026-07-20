#pragma once
#include <string>

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
