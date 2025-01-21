cmake_minimum_required(VERSION 3.20)
project(ptcheck)

set(CMAKE_CXX_STANDARD 20)

# Create the executable
add_executable(ptcheck main.cpp)

# Link libraries
target_link_libraries(ptcheck resolv)

# Set the installation prefix
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")

# Specify what to install
install(TARGETS ptcheck DESTINATION bin)
