// Copyright 2023 Stewart Charles Fisher II

#include "client.h"

bool Client::_validateFilterInput_(const std::string& operation,
                                   const std::string& param) {
  // Look up the operation in the map
  auto it = _filterRequirements_.find(operation);

  // If the operation is not found, alert user
  if (it == _filterRequirements_.end()) {
    throw std::invalid_argument("Error: Invalid filter operation!");
  }

  // Get the requirement for the operation
  const auto& requirement = it->second;

  // Check the parameter type and validate
  switch (requirement.paramType) {
    case ParamType::Integer:
      // Check if all characters are digits
      return std::all_of(param.begin(), param.end(), ::isdigit);
    case ParamType::Double:
      // Check if the string can be a double
      char* end;
      strtod(param.c_str(), &end);
      return end != param.c_str() && *end == '\0';
    case ParamType::String: {
      // Split the expected values and check for a match
      std::istringstream iss(requirement.expectedValue);
      std::string token;
      while (std::getline(iss, token, '|')) {
        if (param == token) {
          return true;
        }
      }
      return false;
    }
    default:
      // Throw an exception otherwise
      throw std::invalid_argument("Error: Unknown parameter type");
  }
}

void Client::operateClient(const std::string& serverAddress,
                           const std::string& imagePath,
                           const std::string& operation,
                           const std::string& param) {
  // Validate the operation and parameter inputs
  if (!_validateFilterInput_(operation, param)) {
    std::cout << "Error: Invalid operation/parameter input!";
    exit(EXIT_FAILURE);
  }

  // Extract server IP and port from the address
  size_t pos = serverAddress.find(':');
  if (pos == std::string::npos) {
    std::cerr << "Error: Invalid server address format!" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string serverIP = serverAddress.substr(0, pos);
  int serverPort = std::stoi(serverAddress.substr(pos + 1));

  // Create a TCP socket
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Error: Socket could not be created!" << std::endl;
    exit(EXIT_FAILURE);
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
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif  // _WIN32
    exit(EXIT_FAILURE);
  }

  // Confirm server connection to the user
  std::cout << "Connected to server." << std::endl;

  // Display the image
  cv::Mat originalImage = cv::imread(imagePath, cv::IMREAD_COLOR);
  if (originalImage.empty()) {
    std::cerr << "Error: Could not read the image file!" << std::endl;
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif  // _WIN32
    exit(EXIT_FAILURE);
  }

  // Display the original image using imshow
  cv::imshow("Original Image", originalImage);
  cv::waitKey(0);
  cv::destroyWindow("Original Image");

  // Send the instruction
  _sendInstruction_(clientSocket, operation, param);

  // Send image
  std::vector<uchar> sendBuffer;
  cv::imencode(".jpg", originalImage, sendBuffer);
  sendImage(clientSocket, sendBuffer);

  // Receive modified image
  std::vector<uchar> receiveBuffer;
  receiveImage(clientSocket, receiveBuffer);

  // Display modified image
  cv::Mat modifiedImage = cv::imdecode(receiveBuffer, cv::IMREAD_COLOR);
  cv::imshow("Modified Image", modifiedImage);
  cv::waitKey(0);

  // Save the modified image
  bool isSaved = cv::imwrite(imagePath, modifiedImage);
  if (!isSaved) {
    std::cerr << "Error: Could not write the image file!" << std::endl;
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif  // _WIN32
    exit(EXIT_FAILURE);
  }

  // Close the client socket
#ifdef _WIN32
  closesocket(clientSocket);
#else
  close(clientSocket);
#endif  // _WIN32
}

void Client::_sendInstruction_(const int socket, const std::string& operation,
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

int main(int argc, char** argv) {
// Initialise Winsock for Windows
#ifdef _WIN32
  WSADATA wasData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cerr << "Error: WSAStartup failed with error: " << result << std::endl;
    return -1;
  }
#endif  // _WIN32

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

  Client client;

  client.operateClient(serverAddress, imagePath, operation, param);

#ifdef _WIN32
  WSACleanup();
#endif  // _WIN32
  return 0;
}
