// Copyright 2023 Stewart Charles Fisher II

#include "server.h"

// Define a function to handle communication with a specific client
void Server::_handleClient_(int clientSocket) {
  // Receive the instruction
  std::string operation, param;
  _receiveInstruction_(clientSocket, operation, param);

  // Receive original image
  std::vector<uchar> receiveBuffer;
  receiveImage(clientSocket, receiveBuffer);

  // Decode the image
  cv::Mat originalImage = cv::imdecode(receiveBuffer, cv::IMREAD_COLOR);

  // Apply a chosen filter
  cv::Mat modifiedImage;
  auto filter = _createFilter_(operation, param);
  filter->applyFilter(originalImage, modifiedImage);

  // Send modified image
  std::vector<uchar> sendBuffer;
  cv::imencode(".jpg", modifiedImage, sendBuffer);
  sendImage(clientSocket, sendBuffer);

  // Close the client socket after handling the communication
#ifdef _WIN32
  closesocket(clientSocket);
#else
  close(clientSocket);
#endif  // _WIN32
}

void Server::operateServer() {
  // Create a TCP socket
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    std::cerr << "Error: Socket could not be created!" << std::endl;
    exit(EXIT_FAILURE);
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
#ifdef _WIN32
    closesocket(serverSocket);
#else
    close(serverSocket);
#endif  // _WIN32
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(serverSocket, 5) == -1) {
    std::cerr << "Error: Connections could not be listened for!" << std::endl;
#ifdef _WIN32
    closesocket(serverSocket);
#else
    close(serverSocket);
#endif  // _WIN32
    exit(EXIT_FAILURE);
  }

  // Determine the number of hardware threads
  int maxThreads = std::thread::hardware_concurrency();

  // Initialise thread pool with the number of threads
  ThreadPool pool(maxThreads);

  while (true) {
    // Accept incoming client connections
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket =
        accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket != -1) {
      // Lock the mutex before accessing the vector
      std::lock_guard<std::mutex> guard(_clientSocketMutex_);

      _clientSockets_.push_back(clientSocket);

      char clientIP[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
      std::cout << "Client connected: " << clientIP << std::endl;

      // Create a thread for the new client using lambda function
      pool.enqueue(
          [this, clientSocket]() { this->_handleClient_(clientSocket); });
    } else {
      std::cerr << "Error: Client connection could not be established!"
                << std::endl;
#ifdef _WIN32
      closesocket(clientSocket);
      closesocket(serverSocket);
#else
      close(clientSocket);
      close(serverSocket);
#endif  // _WIN32
      exit(EXIT_FAILURE);
    }
  }

#ifdef _WIN32
  closesocket(serverSocket);
#else
  close(serverSocket);
#endif  // _WIN32
}

std::unique_ptr<ImageFilter> Server::_createFilter_(
    const std::string& operation, const std::string& param) {
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

void Server::_receiveInstruction_(const int socket, std::string& operation,
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

int main() {
// Initialise Winsock for Windows
#ifdef _WIN32
  WSADATA wasData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cerr << "Error: WSAStartup failed with error: " << result << std::endl;
    return -1;
  }
#endif  // _WIN32

  Server server;
  server.operateServer();
#ifdef _WIN32
  WSACleanup();
#endif  // _WIN32
  return 0;
}
