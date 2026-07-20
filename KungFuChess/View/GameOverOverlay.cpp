#include "GameOverOverlay.h"
#include <algorithm>
#include <string>

void GameOverOverlay::draw(Img& canvas, std::optional<PieceColor> winner) {
    const int w = canvas.width();
    const int h = canvas.height();
    if (w <= 0 || h <= 0) {
        return;
    }

    // Dim the entire board so the match underneath stays faintly visible.
    canvas.fill_rect_alpha(0, 0, w, h, cv::Scalar(0, 0, 0), 0.62);

    // Centered panel sized to the board (square-ish on typical boards).
    const int panelW = std::clamp(static_cast<int>(w * 0.72), 280, w - 40);
    const int panelH = std::clamp(static_cast<int>(h * 0.28), 150, h - 40);
    const int panelX = (w - panelW) / 2;
    const int panelY = (h - panelH) / 2;

    canvas.fill_rect_alpha(panelX, panelY, panelW, panelH, cv::Scalar(18, 22, 28), 0.92);
    canvas.draw_rect(panelX, panelY, panelW, panelH, cv::Scalar(0, 210, 255), 3);
    canvas.draw_rect(panelX + 8, panelY + 8, panelW - 16, panelH - 16, cv::Scalar(0, 140, 180), 1);

    const int centerX = w / 2;
    const int titleY = panelY + static_cast<int>(panelH * 0.38);
    const int winnerY = panelY + static_cast<int>(panelH * 0.70);

    // Soft separator between title and winner line.
    const int sepY = (titleY + winnerY) / 2;
    const int sepW = panelW / 3;
    canvas.fill_rect(centerX - sepW / 2, sepY, sepW, 2, cv::Scalar(0, 180, 220));

    canvas.put_text_centered("Game Over", centerX, titleY, 1.85, cv::Scalar(245, 248, 255), 3);

    std::string winnerLine = "Draw";
    cv::Scalar winnerColor(200, 210, 220);
    if (winner.has_value()) {
        if (*winner == PieceColor::White) {
            winnerLine = "White Wins!";
            winnerColor = cv::Scalar(230, 235, 245);
        } else {
            winnerLine = "Black Wins!";
            winnerColor = cv::Scalar(0, 210, 255);
        }
    }
    canvas.put_text_centered(winnerLine, centerX, winnerY, 1.25, winnerColor, 2);
}
