#pragma once

#include "../Presentation/BoardRenderHints.h"
#include "../Presentation/DisplayTypes.h"
#include "../Presentation/IBoardRenderer.h"
#include "IPieceSpriteProvider.h"
#include "img.h"
#include <memory>
#include <string>

class BoardRenderer : public IBoardRenderer {
public:
    BoardRenderer(std::string boardImagePath, std::unique_ptr<IPieceSpriteProvider> sprites);

    Img render(const DisplayBoard& board, const BoardRenderHints& hints = {}) const override;
    void show(const DisplayBoard& board, const BoardRenderHints& hints = {}) const;

private:
    static const DisplayMotion* findMotion(const BoardRenderHints& hints, int pieceId);
    static const DisplayJump* findJump(const BoardRenderHints& hints, int pieceId);

    void drawPiece(Img& canvas,
                   const DisplayPiece& piece,
                   int cell,
                   int x,
                   int y,
                   long long animElapsed) const;

    std::string boardImagePath_;
    Img boardBackground_;
    std::unique_ptr<IPieceSpriteProvider> sprites_;
};
