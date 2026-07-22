#pragma once

#include "../Presentation/IGameSession.h"

namespace Server {

class TcpMatchHost {
public:
    explicit TcpMatchHost(unsigned short port);

    /** Listen, accept one client, then tick + handle commands until disconnect. */
    void run(IGameSession& session);

private:
    unsigned short port_;
};

} 
