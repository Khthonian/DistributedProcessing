// Copyright 2023 Stewart Charles Fisher II

#include <sys/socket.h>

#include "transmission.h"

void sendImage(const int socket, const std::vector<uchar>& buffer) {
  for (size_t i = 0; i < buffer.size(); i += FRAGMENT_SIZE) {
    size_t fragmentLength =
        std::min(buffer.size() - i, static_cast<size_t>(FRAGMENT_SIZE));
    std::vector<uchar> fragment(buffer.begin() + i,
                                buffer.begin() + i + fragmentLength);

    // Send fragment
    send(socket, fragment.data(), fragment.size(), 0);
  }

  // Shutdown socket
  shutdown(socket, SHUT_WR);
}

void receiveImage(const int socket, std::vector<uchar>& buffer) {
  std::vector<uchar> fragment(FRAGMENT_SIZE);

  while (true) {
    int bytesReceived = recv(socket, fragment.data(), fragment.size(), 0);

    // Check for end-of-transmission
    if (bytesReceived <= 0) {
      break;
    }

    // Append fragment to buffer
    buffer.insert(buffer.end(), fragment.begin(),
                  fragment.begin() + bytesReceived);
  }
}

void sendInstruction(const int socket, const std::string& operation,
                     const std::string& param) {
  // Ensure the strings fit within the buffer size
  if (operation.size() >= 1024 || param.size() >= 1024) {
    std::cerr << "Operation or parameter too long to send" << std::endl;
    return;
  }

  // Send operation
  if (send(socket, operation.c_str(), operation.size(), 0) < 0) {
    std::cerr << "Failed to send operation" << std::endl;
    return;
  }

  // Send parameter
  if (send(socket, param.c_str(), param.size(), 0) < 0) {
    std::cerr << "Failed to send parameter" << std::endl;
    return;
  }
}

void receiveInstruction(const int socket, std::string& operation,
                        std::string& param) {
  char operationBuffer[1024] = {0};
  char paramBuffer[1024] = {0};

  // Receive operation
  int bytesReceived = recv(socket, operationBuffer, sizeof(operationBuffer), 0);
  if (bytesReceived <= 0) {
    // Handle error or closed connection
    return;
  }
  // Convert to std::string
  operation = std::string(operationBuffer, bytesReceived);

  // Receive parameter
  bytesReceived = recv(socket, paramBuffer, sizeof(paramBuffer), 0);
  if (bytesReceived <= 0) {
    // Handle error or closed connection
    return;
  }
  // Convert to std::string
  param = std::string(paramBuffer, bytesReceived);
}
