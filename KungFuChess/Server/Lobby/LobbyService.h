#pragma once

namespace Server {

class LobbyService {
public:
    explicit LobbyService(unsigned short port);
    void run();

private:
    unsigned short port_;
};

} 
