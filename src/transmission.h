// Copyright 2023 Stewart Charles Fisher II

// Include libraries
#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#ifndef SRC_TRANSMISSION_H_
#define SRC_TRANSMISSION_H_

// Define fragment size
const int FRAGMENT_SIZE = 4096;

// Send the image in fragments
void sendImage(const int socket, const std::vector<uchar>& buffer);

// Receive the image in fragments
void receiveImage(const int socket, std::vector<uchar>& buffer);

// Send the instruction
void sendInstruction(const int socket, const std::string& operation,
                     const std::string& param);

void receiveInstruction(const int socket, std::string& operation,
                        std::string& param);

#endif  // SRC_TRANSMISSION_H_
