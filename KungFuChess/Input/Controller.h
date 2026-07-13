#pragma once
#include "Board.h"
#include "Position.h"

// Controller translates user actions into game commands. It does not decide
// chess legality - it only maintains selection state and delegates move
// requests to RuleEngine (and, in a later iteration, to GameEngine).
class Controller {
public:
    void handleClick(int x, int y, Board& board);
    void resetSelection();

private:
    bool hasSelection_ = false;
    Position selected_;
};
