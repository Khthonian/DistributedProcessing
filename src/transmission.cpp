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
  // Prepare length-prefixed messages
  uint32_t opLength = htonl(operation.size());
  uint32_t paramLength = htonl(param.size());

  // Send operation length and operation
  send(socket, &opLength, sizeof(opLength), 0);
  send(socket, operation.c_str(), operation.size(), 0);

  // Send parameter length and parameter
  send(socket, &paramLength, sizeof(paramLength), 0);
  send(socket, param.c_str(), param.size(), 0);
}

void receiveInstruction(const int socket, std::string& operation,
                        std::string& param) {
  uint32_t opLength, paramLength;

  // Receive operation length
  recv(socket, &opLength, sizeof(opLength), 0);
  opLength = ntohl(opLength);

  // Receive operation
  char* opBuffer = new char[opLength + 1];
  recv(socket, opBuffer, opLength, 0);
  opBuffer[opLength] = '\0';  // Null-terminate
  operation = std::string(opBuffer, opLength);
  delete[] opBuffer;

  // Receive parameter length
  recv(socket, &paramLength, sizeof(paramLength), 0);
  paramLength = ntohl(paramLength);

  // Receive parameter
  char* paramBuffer = new char[paramLength + 1];
  recv(socket, paramBuffer, paramLength, 0);

  // Null terminate
  paramBuffer[paramLength] = '\0';
  param = std::string(paramBuffer, paramLength);
  delete[] paramBuffer;
}
