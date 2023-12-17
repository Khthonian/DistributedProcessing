// Copyright 2023 Stewart Charles Fisher II

// Include libraries
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif  // _WIN32

#include <opencv2/core/hal/interface.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#ifndef SRC_PEER_H_
#define SRC_PEER_H_

class Peer {
 protected:
  // Define fragment size
  const int FRAGMENT_SIZE = 4096;

  // Send the image in fragments
  void sendImage(const int socket, const std::vector<uchar>& buffer);

  // Receive the image in fragments
  void receiveImage(const int socket, std::vector<uchar>& buffer);
};
#endif  // SRC_PEER_H_
