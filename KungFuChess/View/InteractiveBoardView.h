#pragma once
#include "../Engine/GameEngine.h"
#include "../Input/Controller.h"
#include "BoardRenderer.h"

class InteractiveBoardView {
public:
    InteractiveBoardView(GameEngine& engine, Controller& controller, BoardRenderer& renderer);

    void run(const std::string& windowName = "KungFuChess");

private:
    GameEngine& engine_;
    Controller& controller_;
    BoardRenderer& renderer_;
};
