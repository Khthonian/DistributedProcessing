// Copyright 2023 Stewart Charles Fisher II

#include <opencv2/core.hpp>
#include <opencv2/core/saturate.hpp>

#include "processing.h"

// Resize filter class

ResizeFilter::ResizeFilter(double multiplier) : _multiplier_(multiplier) {}

void ResizeFilter::applyFilter(cv::Mat& image, cv::Mat& newImage) {
  // Determine the new width and height
  int newWidth = static_cast<int>(image.cols * _multiplier_);
  int newHeight = static_cast<int>(image.rows * _multiplier_);

  // Resize the image
  cv::resize(image, newImage, cv::Size(newWidth, newHeight));
}

// Rotate filter class

RotateFilter::RotateFilter(double angle) : _angle_(angle) {}

void RotateFilter::applyFilter(cv::Mat& image, cv::Mat& newImage) {
  // Determine the centre of rotation
  cv::Point2f centre(image.cols / 2.0F, image.rows / 2.0F);

  // Get the rotation matrix
  cv::Mat rotateMatrix = cv::getRotationMatrix2D(centre, _angle_, 1.0);

  // Determine the bounding rectangle
  cv::Rect2f boundRect =
      cv::RotatedRect(cv::Point2f(), image.size(), _angle_).boundingRect2f();

  // Adjust the rotation matrix to account for translation
  rotateMatrix.at<double>(0, 2) += boundRect.width / 2.0 - centre.x;
  rotateMatrix.at<double>(1, 2) += boundRect.height / 2.0 - centre.y;

  // Rotate the image
  cv::warpAffine(image, newImage, rotateMatrix, boundRect.size());
}

// Flip filter class

FlipFilter::FlipFilter(int flipCode) : _flipCode_(flipCode) {}

void FlipFilter::applyFilter(cv::Mat& image, cv::Mat& newImage) {
  // Perform the flip
  cv::flip(image, newImage, _flipCode_);
}

// Brightness filter class

BrightnessFilter::BrightnessFilter(double alpha) : _alpha_(alpha) {}

void BrightnessFilter::applyFilter(cv::Mat& image, cv::Mat& newImage) {
  // Adjust the brightness
  image.convertTo(newImage, -1, _alpha_, 0);
}

// Contrast filter class

ContrastFilter::ContrastFilter(double beta) : _beta_(beta) {}

void ContrastFilter::applyFilter(cv::Mat& image, cv::Mat& newImage) {
  // Adjust the contrast
  image.convertTo(newImage, -1, 1, _beta_);
}

// Gamma filter class

GammaFilter::GammaFilter(double gamma) : _gamma_(gamma) {}

void GammaFilter::applyFilter(cv::Mat& image, cv::Mat& newImage) {
  cv::Mat lookUp(1, 256, CV_8U);
  uchar* p = lookUp.ptr();
  for (int i = 0; i < 256; i++) {
    p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, _gamma_) * 255.0);
  }

  cv::LUT(image, lookUp, newImage);
}
