#include "InteractiveBoardView.h"
#include "../GameConstants.h"
#include "../Presentation/GameFrame.h"
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

InteractiveBoardView::InteractiveBoardView(IGameSession& session,
                                           Controller& controller,
                                           BoardRenderer& renderer)
    : session_(session), controller_(controller), renderer_(renderer) {
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
        if (deltaMs > Kfc::Timing::kNonPositiveDeltaMs) {
            lastTick = now;
            session_.advanceTime(deltaMs);
            dirty = true;
        }

        if (dirty) {
            const GameFrame frame = captureGameFrame(session_);

            std::optional<CellCoord> selected;
            std::vector<CellCoord> legalMoves;
            if (!frame.gameOver && controller_.hasSelection()) {
                selected = controller_.getSelected();
                legalMoves = session_.legalDestinations(*selected);
            }

            const BoardRenderHints hints = toRenderHints(frame, selected, std::move(legalMoves));
            Img image = renderer_.render(frame.board, hints);
            image.present(windowName);
            dirty = false;
        }

        const int key = Img::waitKeyMs(Kfc::Loop::kFramePollIntervalMs);
        if (key == Kfc::Loop::kEscapeKeyCode) {
            break;
        }
    }

    Img::destroyWindows();
}
