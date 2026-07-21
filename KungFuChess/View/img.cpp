#include "img.h"
#include "../GameConstants.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

Img::Img() {
}

Img& Img::read(const std::string& path,
               const std::pair<int, int>& size,
               bool keep_aspect,
               int interpolation) {
    img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + path);
    }

    if (size.first != Kfc::Grid::kNeutral && size.second != Kfc::Grid::kNeutral) {
        int target_w = size.first;
        int target_h = size.second;
        int h = img.rows;
        int w = img.cols;

        if (keep_aspect) {
            double scale = std::min(static_cast<double>(target_w) / w, 
                                   static_cast<double>(target_h) / h);
            int new_w = static_cast<int>(w * scale);
            int new_h = static_cast<int>(h * scale);
            cv::resize(img, img, cv::Size(new_w, new_h), Kfc::Grid::kNeutral, Kfc::Grid::kNeutral, interpolation);
        } else {
            cv::resize(img, img, cv::Size(target_w, target_h), Kfc::Grid::kNeutral, Kfc::Grid::kNeutral, interpolation);
        }
    }

    return *this;
}

void Img::draw_on(Img& other_img, int x, int y) {
    if (img.empty() || other_img.img.empty()) {
        throw std::runtime_error("Both images must be loaded before drawing.");
    }

    cv::Mat& target = other_img.img;
    const int h = img.rows;
    const int w = img.cols;

    if (x < Kfc::Grid::kNeutral || y < Kfc::Grid::kNeutral || y + h > target.rows || x + w > target.cols) {
        throw std::runtime_error("Image does not fit at the specified position.");
    }

    cv::Mat roi = target(cv::Rect(x, y, w, h));

    if (img.channels() == Kfc::Image::kChannelRgba) {
        std::vector<cv::Mat> srcChannels;
        cv::split(img, srcChannels);

        cv::Mat alpha;
        srcChannels[Kfc::Image::kRgbaAlphaChannelIndex].convertTo(alpha, CV_32FC1, Kfc::Image::kByteNormalize);
        cv::Mat alpha3;
        cv::merge(std::vector<cv::Mat>{ alpha, alpha, alpha }, alpha3);

        cv::Mat srcBgr;
        cv::merge(std::vector<cv::Mat>{ srcChannels[0], srcChannels[1], srcChannels[2] }, srcBgr);

        cv::Mat dstBgr = roi;
        if (roi.channels() == Kfc::Image::kChannelRgba) {
            cv::cvtColor(roi, dstBgr, cv::COLOR_BGRA2BGR);
        } else if (roi.channels() != Kfc::Image::kChannelRgb) {
            throw std::runtime_error("Unsupported target image format for alpha blend.");
        }

        cv::Mat srcF;
        cv::Mat dstF;
        srcBgr.convertTo(srcF, CV_32FC3, Kfc::Image::kByteNormalize);
        dstBgr.convertTo(dstF, CV_32FC3, Kfc::Image::kByteNormalize);

        cv::Mat outF = srcF.mul(alpha3)
            + dstF.mul(cv::Scalar(Kfc::Image::kOpaqueAlpha, Kfc::Image::kOpaqueAlpha, Kfc::Image::kOpaqueAlpha)
                       - alpha3);
        cv::Mat out8;
        outF.convertTo(out8, CV_8UC3, static_cast<double>(Kfc::Image::kByteMax));

        if (roi.channels() == Kfc::Image::kChannelRgba) {
            cv::Mat outBgra;
            cv::cvtColor(out8, outBgra, cv::COLOR_BGR2BGRA);
            outBgra.copyTo(roi);
        } else {
            out8.copyTo(roi);
        }
    } else if (img.channels() == Kfc::Image::kChannelRgb && roi.channels() == Kfc::Image::kChannelRgb) {
        img.copyTo(roi);
    } else if (img.channels() == Kfc::Image::kChannelRgb && roi.channels() == Kfc::Image::kChannelRgba) {
        cv::Mat srcBgra;
        cv::cvtColor(img, srcBgra, cv::COLOR_BGR2BGRA);
        srcBgra.copyTo(roi);
    } else {
        throw std::runtime_error("Unsupported image channel combination for draw_on.");
    }
}

void Img::put_text(const std::string& txt, int x, int y, double font_size,
                   const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

void Img::show() {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }

    present("KungFuChess");
    waitKeyMs(Kfc::Image::kBlockingShowWaitMs);
    destroyWindows();
}

void Img::present(const std::string& windowName) const {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    cv::imshow(windowName, img);
}

void Img::draw_rect(int x, int y, int width, int height,
                    const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    cv::rectangle(img, cv::Rect(x, y, width, height), color, thickness);
}

void Img::fill_rect(int x, int y, int width, int height, const cv::Scalar& color) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    cv::rectangle(img, cv::Rect(x, y, width, height), color, cv::FILLED);
}

void Img::fill_rect_alpha(int x, int y, int width, int height,
                          const cv::Scalar& color, double alpha) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    if (width <= Kfc::Grid::kNeutral || height <= Kfc::Grid::kNeutral) {
        return;
    }

    alpha = std::clamp(alpha, Kfc::Progress::kMin, Kfc::Progress::kMax);
    if (alpha <= Kfc::Progress::kMin) {
        return;
    }
    if (alpha >= Kfc::Progress::kMax) {
        fill_rect(x, y, width, height, color);
        return;
    }

    const cv::Rect bounds(Kfc::Image::kOriginX, Kfc::Image::kOriginY, img.cols, img.rows);
    const cv::Rect roiRect = cv::Rect(x, y, width, height) & bounds;
    if (roiRect.empty()) {
        return;
    }

    cv::Mat roi = img(roiRect);
    if (roi.channels() == Kfc::Image::kChannelRgb) {
        cv::Mat overlay(roi.size(), CV_8UC3, color);
        cv::addWeighted(overlay, alpha, roi, Kfc::Progress::kMax - alpha, Kfc::Progress::kMin, roi);
        return;
    }

    if (roi.channels() == Kfc::Image::kChannelRgba) {
        cv::Mat bgr;
        cv::cvtColor(roi, bgr, cv::COLOR_BGRA2BGR);
        cv::Mat overlay(bgr.size(), CV_8UC3, color);
        cv::Mat blended;
        cv::addWeighted(overlay, alpha, bgr, Kfc::Progress::kMax - alpha, Kfc::Progress::kMin, blended);

        std::vector<cv::Mat> channels;
        cv::split(roi, channels);
        std::vector<cv::Mat> blendedChannels;
        cv::split(blended, blendedChannels);
        channels[0] = blendedChannels[0];
        channels[1] = blendedChannels[1];
        channels[2] = blendedChannels[2];
        cv::merge(channels, roi);
        return;
    }

    throw std::runtime_error("Unsupported image format for fill_rect_alpha.");
}

void Img::put_text_centered(const std::string& txt, int centerX, int centerY,
                            double font_size, const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }

    int baseline = Kfc::Grid::kNeutral;
    const cv::Size size = cv::getTextSize(txt, cv::FONT_HERSHEY_SIMPLEX, font_size, thickness, &baseline);
    const int x = centerX - size.width / Kfc::Ui::kSpriteCenterDivisor;
    const int y = centerY + size.height / Kfc::Ui::kSpriteCenterDivisor;
    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

Img Img::clone() const {
    Img copy;
    if (!img.empty()) {
        copy.img = img.clone();
    }
    return copy;
}

int Img::waitKeyMs(int delayMs) {
    return cv::waitKey(delayMs);
}

void Img::destroyWindows() {
    cv::destroyAllWindows();
} 