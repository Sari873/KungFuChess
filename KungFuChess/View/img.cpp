#include "img.h"
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

    if (size.first != 0 && size.second != 0) {  // Check if size is not empty
        int target_w = size.first;
        int target_h = size.second;
        int h = img.rows;
        int w = img.cols;

        if (keep_aspect) {
            double scale = std::min(static_cast<double>(target_w) / w, 
                                   static_cast<double>(target_h) / h);
            int new_w = static_cast<int>(w * scale);
            int new_h = static_cast<int>(h * scale);
            cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
        } else {
            cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
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

    if (x < 0 || y < 0 || y + h > target.rows || x + w > target.cols) {
        throw std::runtime_error("Image does not fit at the specified position.");
    }

    cv::Mat roi = target(cv::Rect(x, y, w, h));

    if (img.channels() == 4) {
        std::vector<cv::Mat> srcChannels;
        cv::split(img, srcChannels);

        cv::Mat alpha;
        srcChannels[3].convertTo(alpha, CV_32FC1, 1.0 / 255.0);
        cv::Mat alpha3;
        cv::merge(std::vector<cv::Mat>{ alpha, alpha, alpha }, alpha3);

        cv::Mat srcBgr;
        cv::merge(std::vector<cv::Mat>{ srcChannels[0], srcChannels[1], srcChannels[2] }, srcBgr);

        cv::Mat dstBgr = roi;
        if (roi.channels() == 4) {
            cv::cvtColor(roi, dstBgr, cv::COLOR_BGRA2BGR);
        } else if (roi.channels() != 3) {
            throw std::runtime_error("Unsupported target image format for alpha blend.");
        }

        cv::Mat srcF;
        cv::Mat dstF;
        srcBgr.convertTo(srcF, CV_32FC3, 1.0 / 255.0);
        dstBgr.convertTo(dstF, CV_32FC3, 1.0 / 255.0);

        cv::Mat outF = srcF.mul(alpha3) + dstF.mul(cv::Scalar(1.0, 1.0, 1.0) - alpha3);
        cv::Mat out8;
        outF.convertTo(out8, CV_8UC3, 255.0);

        if (roi.channels() == 4) {
            cv::Mat outBgra;
            cv::cvtColor(out8, outBgra, cv::COLOR_BGR2BGRA);
            outBgra.copyTo(roi);
        } else {
            out8.copyTo(roi);
        }
    } else if (img.channels() == 3 && roi.channels() == 3) {
        img.copyTo(roi);
    } else if (img.channels() == 3 && roi.channels() == 4) {
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
    waitKeyMs(0);
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

void Img::put_text_centered(const std::string& txt, int centerX, int centerY,
                            double font_size, const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }

    int baseline = 0;
    const cv::Size size = cv::getTextSize(txt, cv::FONT_HERSHEY_SIMPLEX, font_size, thickness, &baseline);
    const int x = centerX - size.width / 2;
    const int y = centerY + size.height / 2;
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