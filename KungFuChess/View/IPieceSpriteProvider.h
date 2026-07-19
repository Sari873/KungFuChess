#pragma once
#include "img.h"
#include "../Model/Piece.h"

class IPieceSpriteProvider {
public:
    virtual ~IPieceSpriteProvider() = default;
    virtual Img loadForCell(const Piece& piece, int cellSizePx, long long clockMs) const = 0;
};
