// Copyright 2023 Stewart Charles Fisher II

#include "peer.h"

void Peer::sendImage(const int socket, const std::vector<uchar>& buffer) {
  for (size_t i = 0; i < buffer.size(); i += FRAGMENT_SIZE) {
    size_t fragmentLength =
        std::min(buffer.size() - i, static_cast<size_t>(FRAGMENT_SIZE));
    std::vector<uchar> fragment(buffer.begin() + i,
                                buffer.begin() + i + fragmentLength);

    // Send fragment
    send(socket, reinterpret_cast<const char*>(fragment.data()),
         fragment.size(), 0);
  }

// Shutdown socket
#ifdef _WIN32
  shutdown(socket, SD_SEND);
#else
  shutdown(socket, SHUT_WR);
#endif  // _WIN32
}

void Peer::receiveImage(const int socket, std::vector<uchar>& buffer) {
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
