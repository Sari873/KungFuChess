#include "GameOverOverlay.h"
#include <string>

void GameOverOverlay::draw(Img& canvas, std::optional<PieceColor> winner) {
    const int w = canvas.width();
    const int h = canvas.height();
    const int boxH = 140;
    const int boxY = (h - boxH) / 2;

    canvas.fill_rect(0, boxY, w, boxH, cv::Scalar(20, 20, 20));
    canvas.draw_rect(8, boxY + 8, w - 16, boxH - 16, cv::Scalar(0, 220, 255), 2);
    canvas.put_text_centered("Game Over", w / 2, boxY + 52, 1.6, cv::Scalar(255, 255, 255), 3);

    std::string winnerLine = "Draw";
    if (winner.has_value()) {
        winnerLine = (*winner == PieceColor::White) ? "White Wins!" : "Black Wins!";
    }
    canvas.put_text_centered(winnerLine, w / 2, boxY + 100, 1.2, cv::Scalar(0, 220, 255), 2);
}
