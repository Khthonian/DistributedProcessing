// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <arpa/inet.h>
#include <netinet/in.h>
#include <opencv2/core/hal/interface.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "transmission.h"

int main(int argc, char** argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <server_ip:port> <image_path> <operation> <param>"
              << std::endl;
    return -1;
  }

  // Extract command-line arguments
  std::string serverAddress = argv[1];
  std::string imagePath = argv[2];
  std::string operation = argv[3];
  std::string param = argv[4];

  // Extract server IP and port from the address
  size_t pos = serverAddress.find(':');
  if (pos == std::string::npos) {
    std::cerr << "Error: Invalid server address format!" << std::endl;
    return -1;
  }

  std::string serverIP = serverAddress.substr(0, pos);
  int serverPort = std::stoi(serverAddress.substr(pos + 1));

  // Create a TCP socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Error: Socket could not be created!" << std::endl;
    return -1;
  }

  // Prepare the destination server address and port information
  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);
  serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

  // Connect to the server
  if (connect(clientSocket, (struct sockaddr*)&serverAddr,
              sizeof(serverAddr)) == -1) {
    std::cerr << "Error: Server connection could not be established!"
              << std::endl;
    close(clientSocket);
    return -1;
  }

  // Confirm server connection to the user
  std::cout << "Connected to server." << std::endl;

  // Display the image
  cv::Mat originalImage = cv::imread(imagePath, cv::IMREAD_COLOR);
  if (originalImage.empty()) {
    std::cerr << "Error: Could not read the image file!" << std::endl;
    close(clientSocket);
    exit(EXIT_FAILURE);
  }

  // Display the original image using imshow
  cv::imshow("Original Image", originalImage);
  cv::waitKey(0);
  cv::destroyWindow("Original Image");

  // Send image
  std::vector<uchar> sendBuffer;
  cv::imencode(".jpg", originalImage, sendBuffer);
  sendImage(clientSocket, sendBuffer);

  // Receive modified image
  std::vector<uchar> receiveBuffer;
  receiveImage(clientSocket, receiveBuffer);

  cv::Mat modifiedImage = cv::imdecode(receiveBuffer, cv::IMREAD_COLOR);
  cv::imshow("Modified Image", modifiedImage);
  cv::waitKey(0);

  // Close the client socket
  close(clientSocket);

  return 0;
}
