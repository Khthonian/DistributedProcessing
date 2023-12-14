// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <arpa/inet.h>
#include <netinet/in.h>
#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "instruction.h"
#include "processing.h"
#include "threadPool.h"
#include "transmission.h"

// Define a vector to keep track of connected clients
std::vector<int> clientSockets;

// Define a mutex to synchronize access to shared resources
std::mutex clientSocketMutex;

// Define a function to handle communication with a specific client
void handleClient(int clientSocket) {
  // Receive the instruction
  std::string operation, param;
  receiveInstruction(clientSocket, operation, param);
  std::cout << operation << " " << param << std::endl;

  // Receive original image
  std::vector<uchar> receiveBuffer;
  receiveImage(clientSocket, receiveBuffer);

  // Decode the image
  cv::Mat originalImage = cv::imdecode(receiveBuffer, cv::IMREAD_COLOR);

  // Apply a chosen filter
  cv::Mat modifiedImage;
  auto filter = createFilter(operation, param);
  filter->applyFilter(originalImage, modifiedImage);

  // Send modified image
  std::vector<uchar> sendBuffer;
  cv::imencode(".jpg", modifiedImage, sendBuffer);
  sendImage(clientSocket, sendBuffer);

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

  // Initialise thread pool with 4 threads
  ThreadPool pool(4);

  while (true) {
    // Accept incoming client connections
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket != -1) {
      // Lock the mutex before accessing the vector
      std::lock_guard<std::mutex> guard(clientSocketMutex);

      clientSockets.push_back(clientSocket);

      char clientIP[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
      std::cout << "Client connected: " << clientIP << std::endl;

      // Create a thread for the new client
      pool.enqueue(handleClient, clientSocket);
    } else {
      std::cerr << "Error: Client connection could not be established!"
                << std::endl;
      close(clientSocket);
    }
  }

  close(serverSocket);

  return 0;
}
