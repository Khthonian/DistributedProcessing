// Copyright 2023 Stewart Charles Fisher II
#pragma once

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#ifndef SRC_PROCESSING_H_
#define SRC_PROCESSING_H_

#include <opencv2/opencv.hpp>

// Define a base class for all filters
class ImageFilter {
 public:
  // Pure virtual filter application function
  virtual void applyFilter(cv::Mat& image, cv::Mat& newImage) = 0;
};

// Define a derived class for resizing
class ResizeFilter : public ImageFilter {
 private:
  // Define the multiplier
  double _multiplier_;

 public:
  ResizeFilter(double multiplier);

  void applyFilter(cv::Mat& image, cv::Mat& newImage) override;
};

// Define a derived class for rotating
class RotateFilter : public ImageFilter {
 private:
  // Define the rotation angle
  double _angle_;

 public:
  RotateFilter(double angle);

  void applyFilter(cv::Mat& image, cv::Mat& newImage) override;
};

// Define derived class for flipping
class FlipFilter : public ImageFilter {
 private:
  // Define the flip code
  int _flipCode_;

 public:
  FlipFilter(int flipCode);

  void applyFilter(cv::Mat& image, cv::Mat& newImage) override;
};

// Define an abstract derived class for colour adjustment filters
class ColourAdjustFilter : public ImageFilter {
 public:
  // Abstract class
};

// Define derived class for brightness adjustment
class BrightnessFilter : public ColourAdjustFilter {
 private:
  // Define the alpha value
  double _alpha_;

 public:
  BrightnessFilter(double alpha);

  void applyFilter(cv::Mat& image, cv::Mat& newImage) override;
};

// Define derived class for contrast adjustment
class ContrastFilter : public ColourAdjustFilter {
 private:
  // Define the beta value
  double _beta_;

 public:
  ContrastFilter(double beta);

  void applyFilter(cv::Mat& image, cv::Mat& newImage) override;
};

// Define derived class for gamma correction
class GammaFilter : public ColourAdjustFilter {
 private:
  // Define the gamma value
  double _gamma_;

 public:
  GammaFilter(double gamma);

  void applyFilter(cv::Mat& image, cv::Mat& newImage) override;
};

#endif  // SRC_PROCESSING_H_
