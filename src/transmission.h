// Copyright 2023 Stewart Charles Fisher II

#ifndef SRC_TRANSMISSION_H_
#define SRC_TRANSMISSION_H_

#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstddef>
#include <vector>

class Transmission {
 private:
  // Define fragment size
  const int FRAGMENT_SIZE = 4096;

 public:
  // Send the image in fragments
  void sendImage(const int socket, const std::vector<uchar>& buffer);

  // Receive the image in fragments
  void receiveImage(const int socket, std::vector<uchar>& buffer);
};
#endif  // SRC_TRANSMISSION_H_
