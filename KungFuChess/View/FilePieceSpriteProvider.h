#pragma once
#include "IPieceSpriteProvider.h"
#include "PieceAnimationLibrary.h"
#include <string>

class FilePieceSpriteProvider : public IPieceSpriteProvider {
public:
    explicit FilePieceSpriteProvider(std::string assetsRoot);

    Img loadForCell(const Piece& piece, int cellSizePx, long long clockMs) const override;

private:
    static const char* stateFolderFor(PieceState state);

    mutable PieceAnimationLibrary library_;
};
