#include "FilePieceSpriteProvider.h"

FilePieceSpriteProvider::FilePieceSpriteProvider(std::string assetsRoot)
    : library_(std::move(assetsRoot)) {
}

const char* FilePieceSpriteProvider::stateFolderFor(PieceState state) {
    switch (state) {
    case PieceState::Moving:
        return "move";
    case PieceState::Jumping:
        return "jump";
    case PieceState::Idle:
    case PieceState::Captured:
        return "idle";
    }
    return "idle";
}

Img FilePieceSpriteProvider::loadForCell(const Piece& piece, int cellSizePx, long long animElapsedMs) const {
    if (piece.getState() == PieceState::Captured) {
        return Img{};
    }

    const char* folder = stateFolderFor(piece.getState());
    const AnimationClip& clip = library_.clip(piece.toNotation(), folder, cellSizePx);
    if (clip.empty()) {
        return Img{};
    }

    return clip.frameAt(animElapsedMs);
}
