#pragma once
#include <string>

struct MoveResult {
    bool is_accepted;
    std::string reason;

    static MoveResult accepted() { return MoveResult{ true, "ok" }; }
    static MoveResult rejected(const std::string& reason) { return MoveResult{ false, reason }; }
};