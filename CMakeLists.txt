# CMakeLists.txt

cmake_minimum_required(VERSION 3.12)
project(DistributedProcessing)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Set the src directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Server executable
add_executable(server ${SRC_DIR}/server.cpp ${SRC_DIR}/processing.cpp ${SRC_DIR}/processing.h ${SRC_DIR}/peer.cpp ${SRC_DIR}/peer.h ${SRC_DIR}/threadPool.cpp ${SRC_DIR}/threadPool.h)
target_link_libraries(server PRIVATE ${OpenCV_LIBS})

# Client executable
add_executable(client ${SRC_DIR}/client.cpp ${SRC_DIR}/peer.cpp ${SRC_DIR}/peer.h)
target_link_libraries(client PRIVATE ${OpenCV_LIBS})

# Windows-specific compilation
if(WIN32)
  target_link_libraries(server PRIVATE Ws2_32)
  target_link_libraries(client PRIVATE Ws2_32)
endif()

# Set the output directory for the executables
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin")
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin")
