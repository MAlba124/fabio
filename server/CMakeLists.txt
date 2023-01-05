cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

project(pokerServer)

if(${BUILD_TESTS})
    add_subdirectory(test)
endif()

add_executable(Server main.cpp
        server.cpp
        config.cpp
        game.cpp
        player.cpp
)