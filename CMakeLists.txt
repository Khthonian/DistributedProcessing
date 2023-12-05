# CMakeLists.txt

cmake_minimum_required(VERSION 3.12)
project(DistributedProcessing)

# Ensure the build directory exists
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/build")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build")
endif()

# Set the src directory
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Accessory scripts
set(COMMON_SRCS ${SRC_DIR}/processing.cpp)

# Server executable
add_executable(server ${SRC_DIR}/server.cpp ${COMMON_SRCS})

# Client executable
add_executable(client ${SRC_DIR}/client.cpp)

