#pragma once

#include "../Presentation/IGameSession.h"

#include "BoardRenderer.h"

#include "../Input/Controller.h"



class InteractiveBoardView {

public:

    InteractiveBoardView(IGameSession& session, Controller& controller, BoardRenderer& renderer);



    void run(const std::string& windowName = "KungFuChess");



private:

    IGameSession& session_;

    Controller& controller_;

    BoardRenderer& renderer_;

};


