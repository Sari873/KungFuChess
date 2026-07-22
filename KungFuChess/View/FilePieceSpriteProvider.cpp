#include "FilePieceSpriteProvider.h"



FilePieceSpriteProvider::FilePieceSpriteProvider(std::string assetsRoot)

    : library_(std::move(assetsRoot)) {

}



const char* FilePieceSpriteProvider::stateFolderFor(DisplayPieceState state) {

    switch (state) {

    case DisplayPieceState::Moving:

        return "move";

    case DisplayPieceState::Jumping:

        return "jump";

    case DisplayPieceState::Idle:

    case DisplayPieceState::Captured:

        return "idle";

    }

    return "idle";

}



std::string FilePieceSpriteProvider::notationFor(const DisplayPiece& piece) {

    char colorChar = piece.color == DisplayColor::White ? 'w' : 'b';

    char kindChar = '?';

    switch (piece.kind) {

    case DisplayKind::King: kindChar = 'K'; break;

    case DisplayKind::Queen: kindChar = 'Q'; break;

    case DisplayKind::Rook: kindChar = 'R'; break;

    case DisplayKind::Bishop: kindChar = 'B'; break;

    case DisplayKind::Knight: kindChar = 'N'; break;

    case DisplayKind::Pawn: kindChar = 'P'; break;

    }

    return std::string{colorChar, kindChar};

}



Img FilePieceSpriteProvider::loadForCell(const DisplayPiece& piece, int cellSizePx, long long animElapsedMs) const {

    if (piece.state == DisplayPieceState::Captured) {

        return Img{};

    }



    const char* folder = stateFolderFor(piece.state);

    const AnimationClip& clip = library_.clip(notationFor(piece), folder, cellSizePx);

    if (clip.empty()) {

        return Img{};

    }



    return clip.frameAt(animElapsedMs);

}


