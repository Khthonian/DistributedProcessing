// Copyright 2023 Stewart Charles Fisher II

#include <memory>

#include "instruction.h"

std::unordered_map<std::string, FilterRequirement> filterRequirements = {
    {"resize", {ParamType::Double, ""}},
    {"rotate", {ParamType::Double, ""}},
    {"flip", {ParamType::Integer, ""}},
    {"brightness", {ParamType::Double, ""}},
    {"contrast", {ParamType::Double, ""}},
    {"gamma", {ParamType::Double, ""}},
    {"colour", {ParamType::String, "rgb|hsv|grey|ycc|hsl"}},
    {"smooth", {ParamType::String, "gauss|box|sharp"}},
};

bool validateFilterInput(const std::string &operation,
                         const std::string &param) {
  auto it = filterRequirements.find(operation);
  if (it == filterRequirements.end()) {
    throw std::invalid_argument("Invalid filter operation");
  }

  const auto &requirement = it->second;
  switch (requirement.paramType) {
    case ParamType::Integer:
      return std::all_of(param.begin(), param.end(), ::isdigit);
    case ParamType::Double:
      char *end;
      strtod(param.c_str(), &end);
      return end != param.c_str() && *end == '\0';
    case ParamType::String:
      return param == requirement.expectedValue;
    default:
      throw std::invalid_argument("Unknown parameter type");
  }
}

std::unique_ptr<ImageFilter> createFilter(const std::string &operation,
                                          const std::string &param) {
  std::istringstream iss(param);
  double numericParam;
  int intParam;

  // Return the appropriate filter
  if (operation == "resize") {
    iss >> numericParam;
    return std::make_unique<ResizeFilter>(numericParam);
  } else if (operation == "rotate") {
    iss >> numericParam;
    return std::make_unique<RotateFilter>(numericParam);
  } else if (operation == "flip") {
    iss >> intParam;
    return std::make_unique<FlipFilter>(intParam);
  } else if (operation == "brightness") {
    iss >> numericParam;
    return std::make_unique<BrightnessFilter>(numericParam);
  } else if (operation == "contrast") {
    iss >> numericParam;
    return std::make_unique<ContrastFilter>(numericParam);
  } else if (operation == "gamma") {
    iss >> numericParam;
    return std::make_unique<GammaFilter>(numericParam);
  } else if (operation == "colour") {
    if (param == "rgb") {
      return std::make_unique<RGBFilter>();
    } else if (param == "hsv") {
      return std::make_unique<HSVFilter>();
    } else if (param == "grey") {
      return std::make_unique<GreyFilter>();
    } else if (param == "ycc") {
      return std::make_unique<YCCFilter>();
    } else if (param == "hsl") {
      return std::make_unique<HSLFilter>();
    }
  } else if (operation == "smooth") {
    if (param == "gauss") {
      return std::make_unique<GaussianFilter>();
    } else if (param == "box") {
      return std::make_unique<BoxFilter>();
    } else if (param == "sharp") {
      return std::make_unique<SharpFilter>();
    }
  }

  // Handle unusable cases
  return nullptr;
}
