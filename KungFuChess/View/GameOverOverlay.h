#pragma once
#include "../Model/Piece.h"
#include "img.h"
#include <optional>

class GameOverOverlay {
public:
    static void draw(Img& canvas, std::optional<PieceColor> winner);
};
