#pragma once

#include <limits>

namespace Kfc {

namespace Timing {
constexpr long long kMsPerCell = 1000;
constexpr long long kJumpDurationMs = 1000;
constexpr long long kNonPositiveDeltaMs = 0;
constexpr double kMillisecondsPerSecond = 1000.0;
} 

namespace Progress {
constexpr double kMin = 0.0;
constexpr double kMax = 1.0;
constexpr double kComplete = 1.0;

inline double unitInterval(double value) {
    if (value < kMin) {
        return kMin;
    }
    if (value > kMax) {
        return kMax;
    }
    return value;
}

inline double fromElapsedDuration(long long elapsedMs, long long durationMs) {
    if (durationMs <= Timing::kNonPositiveDeltaMs) {
        return kComplete;
    }
    const double ratio = static_cast<double>(elapsedMs) / static_cast<double>(durationMs);
    return unitInterval(ratio);
}
} 

namespace Limits {
constexpr int kMaxPieceIdScan = 128;
constexpr int kFirstAutoPieceId = 1;
constexpr int kPieceNotationTokenLength = 2;
constexpr int kPieceKindCount = 6;
constexpr int kKnightMovePatternCount = 8;
constexpr long long kStationaryOccupantEpochMs = std::numeric_limits<long long>::min() / 4;
} 

namespace Grid {
constexpr int kBackward = -1;
constexpr int kNeutral = 0;
constexpr int kForward = 1;
constexpr int kPawnDoubleStepCells = 2;
constexpr int kKingNeighborMin = -1;
constexpr int kKingNeighborMax = 1;
} 

namespace Ui {
constexpr int kCellSizePx = 100;
constexpr int kCellCenterPixelOffset = 50;
constexpr int kLegalMoveHighlightInsetPx = 8;
constexpr int kLegalMoveHighlightShrinkPx = 16;
constexpr int kLegalMoveOutlineThicknessPx = 2;
constexpr int kSelectionOutlineThicknessPx = 3;
constexpr int kJumpArcMaxLiftPx = 18;
constexpr double kJumpArcWaveScale = 2.0;
constexpr int kSpriteCenterDivisor = 2;

namespace Bgr {
constexpr int kLegalMoveHighlight[3] = { 80, 200, 80 };
constexpr int kSelectionHighlight[3] = { 0, 220, 255 };
} 
} 

namespace Overlay {
constexpr double kBoardDimAlpha = 0.62;
constexpr double kPanelWidthRatio = 0.72;
constexpr int kPanelMinWidthPx = 280;
constexpr int kPanelHorizontalMarginPx = 40;
constexpr double kPanelHeightRatio = 0.28;
constexpr int kPanelMinHeightPx = 150;
constexpr int kPanelVerticalMarginPx = 40;
constexpr double kPanelFillAlpha = 0.92;
constexpr int kPanelOuterBorderThicknessPx = 3;
constexpr int kPanelInnerInsetPx = 8;
constexpr int kPanelInnerShrinkPx = 16;
constexpr int kPanelInnerBorderThicknessPx = 1;
constexpr double kTitleVerticalRatio = 0.38;
constexpr double kWinnerVerticalRatio = 0.70;
constexpr int kSeparatorThicknessPx = 2;
constexpr int kSeparatorWidthDivisor = 3;
constexpr double kTitleFontScale = 1.85;
constexpr int kTitleFontThicknessPx = 3;
constexpr double kWinnerFontScale = 1.25;
constexpr int kWinnerFontThicknessPx = 2;

namespace Bgr {
constexpr int kBoardDim[3] = { 0, 0, 0 };
constexpr int kPanelFill[3] = { 18, 22, 28 };
constexpr int kPanelOuterBorder[3] = { 0, 210, 255 };
constexpr int kPanelInnerBorder[3] = { 0, 140, 180 };
constexpr int kSeparator[3] = { 0, 180, 220 };
constexpr int kTitleText[3] = { 245, 248, 255 };
constexpr int kWinnerDefault[3] = { 200, 210, 220 };
constexpr int kWinnerWhite[3] = { 230, 235, 245 };
constexpr int kWinnerBlack[3] = { 0, 210, 255 };
} 
} 

namespace Loop {
constexpr int kFramePollIntervalMs = 16;
constexpr int kEscapeKeyCode = 27;
} 

namespace Animation {
constexpr int kMaxSpriteFrames = 64;
constexpr int kInitialFrameVectorReserve = 8;
constexpr int kFirstFrameNumber = 1;
constexpr double kDefaultFramesPerSec = 4.0;
constexpr double kMoveStateFramesPerSec = 8.0;
constexpr double kJumpStateFramesPerSec = 10.0;
constexpr int kLoopIndexFloor = 0;
} 

namespace Process {
constexpr int kSuccessExitCode = 0;
constexpr int kErrorExitCode = 1;
} 

namespace Test {
constexpr int kTinyBoardExtent = 3;
constexpr int kSmallBoardRows = 2;
constexpr int kWideBoardCols = 4;
constexpr int kOffScreenClickPx = 9999;
constexpr long long kSingleCellTravelMs = 1000;
constexpr long long kAbsurdWaitMs = 99999;
constexpr int kLastCellIndex = 2;
constexpr int kTwoCellMoveIndex = 2;
constexpr int kPixelJustInsideCell = 99;
constexpr int kPixelCellBoundary = 100;
constexpr int kPixelLastInCell = 299;
constexpr int kPixelPastBoardEdge = 300;
constexpr int kNegativePixel = -1;
constexpr int kProbePixelMid = 50;
constexpr int kNonSquareProbeX = 350;
constexpr int kNonSquareProbeYValid = 150;
constexpr int kNonSquareProbeYPast = 250;
constexpr int kNonSquareProbeXPast = 450;
constexpr int kDiagonalClickX = 250;
constexpr int kDiagonalClickY = 150;
} 

namespace Knight {
constexpr int kLongLeg = 2;
constexpr int kShortLeg = 1;
constexpr int kOffsets[Limits::kKnightMovePatternCount][2] = {
    { -kLongLeg, -kShortLeg }, { -kLongLeg, kShortLeg }, { -kShortLeg, -kLongLeg }, { -kShortLeg, kLongLeg },
    { kShortLeg, -kLongLeg }, { kShortLeg, kLongLeg }, { kLongLeg, -kShortLeg }, { kLongLeg, kShortLeg }
};
} 

namespace Image {
constexpr int kChannelRgb = 3;
constexpr int kChannelRgba = 4;
constexpr int kRgbaAlphaChannelIndex = 3;
constexpr int kByteMax = 255;
constexpr double kByteNormalize = 1.0 / 255.0;
constexpr double kOpaqueAlpha = 1.0;
constexpr int kBlockingShowWaitMs = 0;
constexpr int kOriginX = 0;
constexpr int kOriginY = 0;
} 

} 
