// Copyright 2023 Stewart Charles Fisher II

#ifndef SRC_PROCESSING_H_
#define SRC_PROCESSING_H_

#include <opencv2/opencv.hpp>

class ImageFilters {
 public:
  // Convert the image to grayscale
  static void grayscale(cv::Mat& image);

  // Resize the image
  static void resize(cv::Mat& image, int width, int height);

  // Rotate the image by a given angle (in degrees)
  static void rotate(cv::Mat& image, double angle);

  // Flip the image horizontally or vertically
  static void flip(cv::Mat& image, bool horizontal, bool vertical);

  // Apply Gaussian blur to the image
  static void gaussianBlur(cv::Mat& image, int kernelSize);

  // Apply gamma correction to the image
  static void gammaCorrection(cv::Mat& image, cv::Mat& newImage, double gamma);
};

#endif  // SRC_PROCESSING_H_
