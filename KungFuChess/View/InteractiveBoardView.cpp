#include "InteractiveBoardView.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <optional>

namespace {

struct MouseBridge {
    Controller* controller = nullptr;
    bool* dirty = nullptr;
};

void onMouse(int event, int x, int y, int /*flags*/, void* userdata) {
    if (event != cv::EVENT_LBUTTONDOWN || userdata == nullptr) {
        return;
    }
    auto* bridge = static_cast<MouseBridge*>(userdata);
    bridge->controller->handleClick(x, y);
    *bridge->dirty = true;
}

} 

InteractiveBoardView::InteractiveBoardView(GameEngine& engine,
                                           Controller& controller,
                                           BoardRenderer& renderer)
    : engine_(engine), controller_(controller), renderer_(renderer) {
}

void InteractiveBoardView::run(const std::string& windowName) {
    bool dirty = true;
    MouseBridge bridge{ &controller_, &dirty };

    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(windowName, onMouse, &bridge);

    auto lastTick = std::chrono::steady_clock::now();

    while (true) {
        const auto now = std::chrono::steady_clock::now();
        const auto deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick).count();
        if (deltaMs > 0) {
            lastTick = now;
            engine_.advanceTime(deltaMs);
            dirty = true; // idle + move frame cycling
        }

        if (dirty) {
            BoardRenderHints hints;
            hints.clockMs = engine_.getClockMs();
            hints.motions = engine_.activeMotions();
            hints.gameOver = engine_.isGameOver();
            hints.winner = engine_.winner();
            if (!hints.gameOver && controller_.hasSelection()) {
                hints.selected = controller_.getSelected();
                hints.legalMoves = engine_.legalDestinations(*hints.selected);
            }
            Img frame = renderer_.render(engine_.getBoard(), hints);
            frame.present(windowName);
            dirty = false;
        }

        const int key = Img::waitKeyMs(16);
        if (key == 27) {
            break;
        }
    }

    Img::destroyWindows();
}
