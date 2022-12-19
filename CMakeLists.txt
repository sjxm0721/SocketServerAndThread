cmake_minimum_required(VERSION 3.23)
project(SocketServer2)

set(CMAKE_CXX_STANDARD 11)

link_libraries(pthread)

add_executable(SocketServer2 server.cpp)
