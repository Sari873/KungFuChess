#include "GameConstants.h"
#include "Server/Lobby/LobbyService.h"
#include <iostream>

int main() {
    try {
        Server::LobbyService lobby(Kfc::Server::kDefaultPort);
        lobby.run();
        return Kfc::Process::kSuccessExitCode;
    } catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << '\n';
        return Kfc::Process::kErrorExitCode;
    }
}
