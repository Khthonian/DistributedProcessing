// Copyright 2023 Stewart Charles Fisher II

#include "processing.h"

void ImageFilters::grayscale(cv::Mat& image) {
  cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
}

void ImageFilters::resize(cv::Mat& image, int width, int height) {
  cv::resize(image, image, cv::Size(width, height));
}

void ImageFilters::rotate(cv::Mat& image, double angle) {
  cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
  cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
  cv::warpAffine(image, image, rotationMatrix, image.size());
}

void ImageFilters::flip(cv::Mat& image, bool horizontal, bool vertical) {
  if (horizontal) cv::flip(image, image, 1);  // Flip horizontally
  if (vertical) cv::flip(image, image, 0);    // Flip vertically
}

void ImageFilters::gaussianBlur(cv::Mat& image, int kernelSize) {
  cv::GaussianBlur(image, image, cv::Size(kernelSize, kernelSize), 0);
}

void ImageFilters::gammaCorrection(cv::Mat& image, cv::Mat& newImage,
                                   double gamma) {
  cv::Mat lookupTable(1, 256, CV_8U);
  uchar* ptr = lookupTable.ptr();

  for (int i = 0; i < 256; ++i)
    ptr[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

  cv::LUT(image, lookupTable, newImage);
}
