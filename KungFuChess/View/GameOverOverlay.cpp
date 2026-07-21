#include "GameOverOverlay.h"
#include "../GameConstants.h"
#include <algorithm>
#include <string>

namespace {

cv::Scalar bgr(const int (&components)[3]) {
    return cv::Scalar(components[0], components[1], components[2]);
}

} 

void GameOverOverlay::draw(Img& canvas, std::optional<DisplayColor> winner) {
    const int w = canvas.width();
    const int h = canvas.height();
    if (w <= Kfc::Grid::kNeutral || h <= Kfc::Grid::kNeutral) {
        return;
    }

    canvas.fill_rect_alpha(Kfc::Grid::kNeutral, Kfc::Grid::kNeutral, w, h,
                           bgr(Kfc::Overlay::Bgr::kBoardDim), Kfc::Overlay::kBoardDimAlpha);

    const int panelW = std::clamp(static_cast<int>(w * Kfc::Overlay::kPanelWidthRatio),
                                  Kfc::Overlay::kPanelMinWidthPx,
                                  w - Kfc::Overlay::kPanelHorizontalMarginPx);
    const int panelH = std::clamp(static_cast<int>(h * Kfc::Overlay::kPanelHeightRatio),
                                  Kfc::Overlay::kPanelMinHeightPx,
                                  h - Kfc::Overlay::kPanelVerticalMarginPx);
    const int panelX = (w - panelW) / Kfc::Ui::kSpriteCenterDivisor;
    const int panelY = (h - panelH) / Kfc::Ui::kSpriteCenterDivisor;

    canvas.fill_rect_alpha(panelX, panelY, panelW, panelH,
                           bgr(Kfc::Overlay::Bgr::kPanelFill), Kfc::Overlay::kPanelFillAlpha);
    canvas.draw_rect(panelX, panelY, panelW, panelH,
                     bgr(Kfc::Overlay::Bgr::kPanelOuterBorder),
                     Kfc::Overlay::kPanelOuterBorderThicknessPx);
    canvas.draw_rect(panelX + Kfc::Overlay::kPanelInnerInsetPx,
                     panelY + Kfc::Overlay::kPanelInnerInsetPx,
                     panelW - Kfc::Overlay::kPanelInnerShrinkPx,
                     panelH - Kfc::Overlay::kPanelInnerShrinkPx,
                     bgr(Kfc::Overlay::Bgr::kPanelInnerBorder),
                     Kfc::Overlay::kPanelInnerBorderThicknessPx);

    const int centerX = w / Kfc::Ui::kSpriteCenterDivisor;
    const int titleY = panelY + static_cast<int>(panelH * Kfc::Overlay::kTitleVerticalRatio);
    const int winnerY = panelY + static_cast<int>(panelH * Kfc::Overlay::kWinnerVerticalRatio);

    const int sepY = (titleY + winnerY) / Kfc::Ui::kSpriteCenterDivisor;
    const int sepW = panelW / Kfc::Overlay::kSeparatorWidthDivisor;
    canvas.fill_rect(centerX - sepW / Kfc::Ui::kSpriteCenterDivisor, sepY, sepW,
                     Kfc::Overlay::kSeparatorThicknessPx, bgr(Kfc::Overlay::Bgr::kSeparator));

    canvas.put_text_centered("Game Over", centerX, titleY, Kfc::Overlay::kTitleFontScale,
                             bgr(Kfc::Overlay::Bgr::kTitleText), Kfc::Overlay::kTitleFontThicknessPx);

    std::string winnerLine = "Draw";
    cv::Scalar winnerColor = bgr(Kfc::Overlay::Bgr::kWinnerDefault);
    if (winner.has_value()) {
        if (*winner == DisplayColor::White) {
            winnerLine = "White Wins!";
            winnerColor = bgr(Kfc::Overlay::Bgr::kWinnerWhite);
        } else {
            winnerLine = "Black Wins!";
            winnerColor = bgr(Kfc::Overlay::Bgr::kWinnerBlack);
        }
    }
    canvas.put_text_centered(winnerLine, centerX, winnerY, Kfc::Overlay::kWinnerFontScale,
                             winnerColor, Kfc::Overlay::kWinnerFontThicknessPx);
}
