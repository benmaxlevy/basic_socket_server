cmake_minimum_required(VERSION 3.17)
project(socket_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server_socket main.cpp args_check/check.h args_check/check.cpp)