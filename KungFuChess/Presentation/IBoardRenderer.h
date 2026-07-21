#pragma once

#include "BoardRenderHints.h"
#include "DisplayTypes.h"
#include "img.h"

class IBoardRenderer {
public:
    virtual ~IBoardRenderer() = default;
    virtual Img render(const DisplayBoard& board, const BoardRenderHints& hints = {}) const = 0;
};
