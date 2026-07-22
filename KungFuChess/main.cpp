#include "View/BoardRenderer.h"
#include "View/FilePieceSpriteProvider.h"
#include "View/InteractiveBoardView.h"
#include "Adapter/GameEngineSession.h"
#include "Client/LobbyClient.h"
#include "Client/NetworkGameSession.h"
#include "GameConstants.h"
#include "Model/StandardChessSetup.h"
#include "Model/GameState.h"
#include "Engine/GameEngine.h"
#include "Input/Controller.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

namespace {

void printUsage() {
    std::cout
        << "Usage:\n"
        << "  KungFuChess.exe                              offline local game\n"
        << "  KungFuChess.exe --connect [host] [port]      lobby then online match\n"
        << "  KungFuChess.exe --connect [host] [port] [name] [rating]\n";
}

void runWithSession(IGameSession& session) {
    Controller controller(session);
    BoardRenderer renderer(
        "pictures/board.png",
        std::make_unique<FilePieceSpriteProvider>("pictures/pieces_mine"));
    std::cout << "Select a piece (green = targets). Re-click it to jump. ESC quits.\n";
    InteractiveBoardView view(session, controller, renderer);
    view.run();
}

} 

int main(int argc, char** argv) {
    try {
        if (argc >= 2 && std::string(argv[1]) == "--help") {
            printUsage();
            return Kfc::Process::kSuccessExitCode;
        }

        if (argc >= 2 && std::string(argv[1]) == "--connect") {
            const std::string host = (argc >= 3) ? argv[2] : "127.0.0.1";
            const unsigned short port =
                (argc >= 4) ? static_cast<unsigned short>(std::stoi(argv[3]))
                            : Kfc::Server::kDefaultPort;
            const std::string name = (argc >= 5) ? argv[4] : "Player";
            const int rating =
                (argc >= 6) ? std::stoi(argv[5]) : Kfc::Server::kDefaultPlayerRating;

            std::cout << "Connecting to lobby " << host << ":" << port << "...\n";
            LobbyClient lobby(host, port);
            auto session = lobby.runMenu(name, rating);

            for (int i = 0; i < 200 && !session->hasFrame(); ++i) {
                session->advanceTime(0);
                std::this_thread::sleep_for(std::chrono::milliseconds(Kfc::Server::kSocketPollMs));
            }
            if (!session->hasFrame()) {
                std::cerr << "Timed out waiting for first GameFrame from server.\n";
                return Kfc::Process::kErrorExitCode;
            }

            runWithSession(*session);
            return Kfc::Process::kSuccessExitCode;
        }

        GameEngine engine{ GameState(StandardChessSetup::create()) };
        GameEngineSession session(engine);
        runWithSession(session);
        return Kfc::Process::kSuccessExitCode;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return Kfc::Process::kErrorExitCode;
    }
}
