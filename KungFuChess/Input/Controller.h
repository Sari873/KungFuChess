#pragma once
#include "../Engine/GameEngine.h"
#include "../Model/Position.h"

class Controller {
public:
    explicit Controller(GameEngine& engine);

    void handleClick(int x, int y);
    void resetSelection();

    bool hasSelection() const;
    Position getSelected() const;

private:
    GameEngine& engine_;
    bool hasSelection_ = false;
    Position selected_;
};
