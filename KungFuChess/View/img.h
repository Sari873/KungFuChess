#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>

class Img {
public:
    Img();
    
    /**
     * Load image from path and optionally resize.
     * 
     * @param path Image file to load
     * @param size Target size in pixels (width, height). If empty, keep original
     * @param keep_aspect If true, shrink so the longer side fits size while preserving aspect ratio
     * @param interpolation OpenCV interpolation flag (e.g., cv::INTER_AREA for shrink, cv::INTER_LINEAR for enlarge)
     * @return Reference to this object for method chaining
     */
    Img& read(const std::string& path,
              const std::pair<int, int>& size = {},
              bool keep_aspect = false,
              int interpolation = cv::INTER_AREA);
    
    /**
     * Draw this image onto another image at position (x, y)
     * 
     * @param other_img The target image to draw on
     * @param x X coordinate for top-left corner
     * @param y Y coordinate for top-left corner
     */
    void draw_on(Img& other_img, int x, int y);
    
    /**
     * Put text on the image
     * 
     * @param txt Text to draw
     * @param x X coordinate for text position
     * @param y Y coordinate for text position (baseline)
     * @param font_size Font scale factor
     * @param color Text color (BGR or BGRA)
     * @param thickness Text thickness
     */
    void put_text(const std::string& txt, int x, int y, double font_size,
                  const cv::Scalar& color = cv::Scalar(255, 255, 255, 255),
                  int thickness = 1);
    
    /**
     * Display the image in a window (blocks until a key is pressed).
     */
    void show();

    /** Non-blocking frame present for interactive loops. */
    void present(const std::string& windowName = "KungFuChess") const;

    void draw_rect(int x, int y, int width, int height,
                   const cv::Scalar& color = cv::Scalar(0, 255, 255),
                   int thickness = 2);

    void fill_rect(int x, int y, int width, int height, const cv::Scalar& color);

    /** Blend a solid BGR color over a rectangle using alpha in [0, 1]. */
    void fill_rect_alpha(int x, int y, int width, int height,
                         const cv::Scalar& color, double alpha);

    void put_text_centered(const std::string& txt, int centerX, int centerY,
                           double font_size,
                           const cv::Scalar& color = cv::Scalar(255, 255, 255),
                           int thickness = 2);

    /** Deep copy — safe to draw on without mutating the source. */
    Img clone() const;

    int width() const { return img.empty() ? 0 : img.cols; }
    int height() const { return img.empty() ? 0 : img.rows; }

    static int waitKeyMs(int delayMs);
    static void destroyWindows();

    bool is_loaded() const { return !img.empty(); }

private:
    cv::Mat img;
}; 