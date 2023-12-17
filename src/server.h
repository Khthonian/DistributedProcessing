// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <arpa/inet.h>
#include <netinet/in.h>
#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "processing.h"
#include "threadPool.h"
#include "transmission.h"

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

class Server {
 private:
  // Define a vector to keep track of connected clients
  std::vector<int> clientSockets;

  // Define a mutex to synchronise access to shared resources
  std::mutex clientSocketMutex;

  // Define a function to handle communication with a specific client
  void handleClient(int clientSocket);

  // Define a factory function to create filter objects
  std::unique_ptr<ImageFilter> createFilter(const std::string& operation,
                                            const std::string& param);

 public:
  // Define a function to manage server operation
  void operateServer();
};

#endif  // SRC_SERVER_H_
