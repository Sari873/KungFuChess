#include "InteractiveBoardView.h"

#include "../GameConstants.h"
#include "../Presentation/BoardRenderHints.h"

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

            BoardRenderHints hints;

            hints.clockMs = session_.clockMs();

            hints.motions = session_.activeMotions();

            hints.jumps = session_.activeJumps();

            hints.gameOver = session_.isGameOver();

            hints.winner = session_.winner();

            if (!hints.gameOver && controller_.hasSelection()) {

                hints.selected = controller_.getSelected();

                hints.legalMoves = session_.legalDestinations(*hints.selected);

            }

            Img frame = renderer_.render(session_.board(), hints);

            frame.present(windowName);

            dirty = false;

        }



        const int key = Img::waitKeyMs(Kfc::Loop::kFramePollIntervalMs);

        if (key == Kfc::Loop::kEscapeKeyCode) {

            break;

        }

    }



    Img::destroyWindows();

}


