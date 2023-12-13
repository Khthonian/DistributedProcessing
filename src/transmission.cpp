// Copyright 2023 Stewart Charles Fisher II

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
  // Send operation
  send(socket, operation.c_str(), operation.size(), 0);

  // Send parameter
  send(socket, param.c_str(), param.size(), 0);
}

void receiveInstruction(const int socket, std::string& operation,
                        std::string& param) {
  // Receive operation
  int bytesReceived = recv(socket, operation.data(), operation.size(), 0);

  // Receive parameter
  bytesReceived = recv(socket, param.data(), param.size(), 0);
}
