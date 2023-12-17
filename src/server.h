// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <memory>
#include <mutex>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "peer.h"
#include "processing.h"
#include "threadPool.h"

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

class Server : public Peer {
 private:
  // Define a vector to keep track of connected clients
  std::vector<int> _clientSockets_;

  // Define a mutex to synchronise access to shared resources
  std::mutex _clientSocketMutex_;

  // Define a function to handle communication with a specific client
  void _handleClient_(int clientSocket);

  // Define a function to receive instructions
  void _receiveInstruction_(const int socket, std::string& operation,
                            std::string& param);

  // Define a factory function to create filter objects
  std::unique_ptr<ImageFilter> _createFilter_(const std::string& operation,
                                              const std::string& param);

 public:
  // Define a function to manage server operation
  void operateServer();
};

#endif  // SRC_SERVER_H_
