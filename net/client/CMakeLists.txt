cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS log)

add_library(libclient client.cpp)

target_link_libraries(libclient ${Boost_LIBRARIES})