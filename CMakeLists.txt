cmake_minimum_required(VERSION 3.20)
project(dnsverificationcpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(dnsverificationcpp main.cpp)
target_link_libraries(dnsverificationcpp resolv)