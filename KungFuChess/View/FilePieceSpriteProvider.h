#pragma once

#include "IPieceSpriteProvider.h"

#include "PieceAnimationLibrary.h"

#include <string>



class FilePieceSpriteProvider : public IPieceSpriteProvider {

public:

    explicit FilePieceSpriteProvider(std::string assetsRoot);



    Img loadForCell(const DisplayPiece& piece, int cellSizePx, long long animElapsedMs) const override;



private:

    static const char* stateFolderFor(DisplayPieceState state);

    static std::string notationFor(const DisplayPiece& piece);



    mutable PieceAnimationLibrary library_;

};


