// Copyright 2023 Stewart Charles Fisher II

#include "server.h"

// Define a function to handle communication with a specific client
void Server::handleClient(int clientSocket) {
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

void Server::operateServer() {
  // Create a TCP socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Error: Socket could not be created!" << std::endl;
    exit(-1);
  }

  // Confirm server launch to user
  std::cout << "Server started. Waiting for connections..." << std::endl;

  // Bind the server socket to a specific port
  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(12345);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) ==
      -1) {
    std::cerr << "Error: Socket could not be bound!" << std::endl;
    close(serverSocket);
    exit(-1);
  }

  // Listen for incoming connections
  if (listen(serverSocket, 5) == -1) {
    std::cerr << "Error: Connections could not be listened for!" << std::endl;
    close(serverSocket);
    exit(-1);
  }

  // Initialise thread pool with 4 threads
  ThreadPool pool(4);

  while (true) {
    // Accept incoming client connections
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket != -1) {
      // Lock the mutex before accessing the vector
      std::lock_guard<std::mutex> guard(clientSocketMutex);

      clientSockets.push_back(clientSocket);

      char clientIP[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
      std::cout << "Client connected: " << clientIP << std::endl;

      // Create a thread for the new client using lambda function
      pool.enqueue(
          [this, clientSocket]() { this->handleClient(clientSocket); });
    } else {
      std::cerr << "Error: Client connection could not be established!"
                << std::endl;
      close(clientSocket);
    }
  }

  close(serverSocket);
}

std::unique_ptr<ImageFilter> Server::createFilter(const std::string &operation,
                                                  const std::string &param) {
  std::istringstream iss(param);
  double numericParam;
  int intParam;

  // Return the appropriate filter
  if (operation == "resize") {
    iss >> numericParam;
    return std::make_unique<ResizeFilter>(numericParam);
  } else if (operation == "rotate") {
    iss >> numericParam;
    return std::make_unique<RotateFilter>(numericParam);
  } else if (operation == "flip") {
    iss >> intParam;
    return std::make_unique<FlipFilter>(intParam);
  } else if (operation == "brightness") {
    iss >> numericParam;
    return std::make_unique<BrightnessFilter>(numericParam);
  } else if (operation == "contrast") {
    iss >> numericParam;
    return std::make_unique<ContrastFilter>(numericParam);
  } else if (operation == "gamma") {
    iss >> numericParam;
    return std::make_unique<GammaFilter>(numericParam);
  } else if (operation == "colour") {
    if (param == "rgb") {
      return std::make_unique<RGBFilter>();
    } else if (param == "hsv") {
      return std::make_unique<HSVFilter>();
    } else if (param == "grey") {
      return std::make_unique<GreyFilter>();
    } else if (param == "ycc") {
      return std::make_unique<YCCFilter>();
    } else if (param == "hsl") {
      return std::make_unique<HSLFilter>();
    }
  } else if (operation == "smooth") {
    if (param == "gauss") {
      return std::make_unique<GaussianFilter>();
    } else if (param == "box") {
      return std::make_unique<BoxFilter>();
    } else if (param == "sharp") {
      return std::make_unique<SharpFilter>();
    }
  }

  // Handle unusable cases
  return nullptr;
}

int main() {
  Server server;
  server.operateServer();
  return 0;
}
