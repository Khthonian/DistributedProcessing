// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <arpa/inet.h>
#include <netinet/in.h>
#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "peer.h"

#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

// Define an enumeration for parameter types
enum class ParamType {
  Double,
  Integer,
  String,
};

// Define s struct for the filter requirements
struct FilterRequirement {
  ParamType paramType;
  std::string expectedValue;
};

class Client : public Peer {
 private:
  // Define a map to hold the requirements for each filter
  std::unordered_map<std::string, FilterRequirement> _filterRequirements_ = {
      {"resize", {ParamType::Double, ""}},
      {"rotate", {ParamType::Double, ""}},
      {"flip", {ParamType::Integer, ""}},
      {"brightness", {ParamType::Double, ""}},
      {"contrast", {ParamType::Double, ""}},
      {"gamma", {ParamType::Double, ""}},
      {"colour", {ParamType::String, "rgb|hsv|grey|ycc|hsl"}},
      {"smooth", {ParamType::String, "gauss|box|sharp"}},
  };

  // Define a function to validate the user input
  bool _validateFilterInput_(const std::string& operation,
                             const std::string& param);

  // Send the instruction
  void _sendInstruction_(const int socket, const std::string& operation,
                         const std::string& param);

 public:
  // Define a function to manage client operation
  void operateClient(const std::string& serverAddress,
                     const std::string& imagePath, const std::string& operation,
                     const std::string& param);
};

#endif  // SRC_CLIENT_H_
