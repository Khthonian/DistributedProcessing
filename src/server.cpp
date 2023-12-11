// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <arpa/inet.h>
#include <netinet/in.h>
#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <mutex>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

#include "processing.h"

// Define the max amount of clients
const int MAX_CLIENTS = 100;

// Define fragment size
const int FRAGMENT_SIZE = 4096;

// Define a mutex to synchronize access to shared resources
std::mutex mutex;

// Define a function to handle communication with a specific client
void handleClient(int clientSocket) {
  std::vector<uchar> completeImage;
  std::vector<uchar> fragment(FRAGMENT_SIZE);

  while (true) {
    int bytesReceived = recv(clientSocket, fragment.data(), fragment.size(), 0);

    // Check for end-of-transmission
    if (bytesReceived <= 0) {
      break;
    }

    // Append fragment to complete data
    completeImage.insert(completeImage.end(), fragment.begin(),
                         fragment.begin() + bytesReceived);
  }

  // Decode the image
  cv::Mat originalImage = cv::imdecode(completeImage, cv::IMREAD_COLOR);
  cv::imshow("Original Image: Server", originalImage);
  cv::waitKey(0);
  cv::destroyWindow("Original Image: Server");

  // Initialise ImageFilters object
  ImageFilters imageFilters;

  // Apply a gamma change
  cv::Mat modifiedImage;
  double gammaValue = 0.5;
  imageFilters.gammaCorrection(originalImage, modifiedImage, gammaValue);

  // Encode the modified image
  std::vector<uchar> sendBuffer;
  cv::imencode(".jpg", modifiedImage, sendBuffer);

  // Send the modified image back
  send(clientSocket, sendBuffer.data(), sendBuffer.size(), 0);

  // Close the client socket after handling the communication
  close(clientSocket);
}

int main() {
  // Create a TCP socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Error: Socket could not be created!" << std::endl;
    return -1;
  }

  // Confirm server launch to user
  std::cout << "Server started. Waiting for connections..." << std::endl;

  // Bind the server socket to a specific port
  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(12345);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) ==
      -1) {
    std::cerr << "Error: Socket could not be bound!" << std::endl;
    close(serverSocket);
    return -1;
  }

  // Listen for incoming connections
  if (listen(serverSocket, 5) == -1) {
    std::cerr << "Error: Connections could not be listened for!" << std::endl;
    close(serverSocket);
    return -1;
  }

  std::vector<int> clientSockets;  // To keep track of connected clients

  while (true) {
    // Accept incoming client connections
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket != -1) {
      if (clientSockets.size() < MAX_CLIENTS) {
        clientSockets.push_back(clientSocket);

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        std::cout << "Client connected: " << clientIP << std::endl;

        // Create a thread for the new client
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
      } else {
        std::cerr << "Error: Client connection could not be established!"
                  << std::endl;
        close(clientSocket);
      }
    }
  }

  return 0;
}
