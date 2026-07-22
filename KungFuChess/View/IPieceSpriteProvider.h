#pragma once

#include "img.h"

#include "../Presentation/DisplayTypes.h"



class IPieceSpriteProvider {

public:

    virtual ~IPieceSpriteProvider() = default;

    virtual Img loadForCell(const DisplayPiece& piece, int cellSizePx, long long clockMs) const = 0;

};


