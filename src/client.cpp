// Copyright 2023 Stewart Charles Fisher II

// Import libraries
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

// Define a message that will trigger a disconnect
const char* DISCONNECT_MESSAGE = "/disconnect";

int main() {
  // Create a TCP socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Error: Socket could not be created!" << std::endl;
    return -1;
  }

  // Prepare the destination server address and port information
  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(12345);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

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

  while (true) {
    // Get the message from the user
    std::cout << "Enter the message to send: ";
    std::string message;
    std::getline(std::cin, message);

    // Check if the user requested a disconnect
    if (message == DISCONNECT_MESSAGE) {
      std::cout << "Disconnected from server.";
      close(clientSocket);
      break;
    }

    // Use the send() function to send the message to the server
    ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
    if (bytesSent == -1) {
      std::cerr << "Error : Message could not be sent!" << std::endl;
      close(clientSocket);
      break;
    }

    // Receive a response message from the server
    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
      std::cerr << "Error: Response could not be received!" << std::endl;
      close(clientSocket);
      break;
    }

    // Print the response message
    buffer[bytesReceived] = '\0';  // Null-terminate the received data
    std::cout << "Response received from server: " << buffer << std::endl;
  }

  return 0;
}
