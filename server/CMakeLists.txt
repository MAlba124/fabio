cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

project(pokerServer)

add_subdirectory(test)

add_executable(Server main.cpp
        server.cpp
)