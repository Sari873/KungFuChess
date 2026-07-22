#pragma once

#include "IFrameSink.h"
#include <iostream>

namespace Server {

class StdoutFrameSink final : public IFrameSink {
public:
    void publish(const GameFrame& /*frame*/, const std::string& encoded) override {
        std::cout << encoded << '\n';
        std::cout.flush();
    }
};

} 
