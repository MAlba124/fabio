cmake_minimum_required(VERSION 3.24)

project(test)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}_client test_client.cpp)