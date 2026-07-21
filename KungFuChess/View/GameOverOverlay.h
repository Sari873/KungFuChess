#pragma once
#include "../Presentation/DisplayTypes.h"
#include "img.h"
#include <optional>

class GameOverOverlay {
public:
    static void draw(Img& canvas, std::optional<DisplayColor> winner);
};
