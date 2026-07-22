#pragma once

#include "../Presentation/GameFrame.h"
#include <string>

namespace Server {

/** Destination for serialized match frames (stdout now; TCP later). */
class IFrameSink {
public:
    virtual ~IFrameSink() = default;
    virtual void publish(const GameFrame& frame, const std::string& encoded) = 0;
};

} 
