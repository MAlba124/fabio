cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

project(pokerServer)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS log_setup log)

add_executable(Server main.cpp
        server.cpp
        config.cpp
        game.cpp
        player.cpp
        log.cpp
        games.cpp
)

target_link_libraries(Server ${Boost_LIBRARIES})