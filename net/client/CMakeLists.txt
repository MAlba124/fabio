cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

add_library(client client.cpp)