#include "View/BoardRenderer.h"
#include "View/FilePieceSpriteProvider.h"
#include "View/InteractiveBoardView.h"
#include "Model/StandardChessSetup.h"
#include "Model/GameState.h"
#include "Engine/GameEngine.h"
#include "Input/Controller.h"
#include <iostream>
#include <memory>

int main() {
    try {
        GameEngine engine{ GameState(StandardChessSetup::create()) };
        Controller controller(engine);

        BoardRenderer renderer(
            "pictures/board.png",
            std::make_unique<FilePieceSpriteProvider>("pictures/pieces_mine"));

        std::cout << "Select a piece (green = targets). Re-click it to jump. ESC quits.\n";
        InteractiveBoardView view(engine, controller, renderer);
        view.run();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
