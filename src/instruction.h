// Copyright 2023 Stewart Charles Fisher II

// Include libraries
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "processing.h"

#ifndef SRC_INSTRUCTION_H_
#define SRC_INSTRUCTION_H_

// Define an enumeration for parameter types
enum class ParamType {
  Double,
  Integer,
  String,
};

// Define a struct for the filter requirements
struct FilterRequirement {
  ParamType paramType;
  std::string expectedValue;
};

// Define a map to hold the requirements for each filter
extern std::unordered_map<std::string, FilterRequirement> filterRequirements;

// Define a function to validate the user input
bool validateFilterInput(const std::string& operation,
                         const std::string& param);

// Define a factory function to create filter objects
std::unique_ptr<ImageFilter> createFilter(const std::string& operation,
                                          const std::string& param);

// Define a function to apply the chosen filter to the image
void applyChosenFilter(const std::string& operation, const std::string& param,
                       cv::Mat& image);

#endif  // SRC_INSTRUCTION_H_
