cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

project(fabioClient)

find_package(SDL2 REQUIRED)
include_directories(fabioClient ${SDL2_INCLUDE_DIRS})

add_executable(Client main.cpp
        imgui-docking/imgui.cpp
        imgui-docking/imgui_draw.cpp
        imgui-docking/imgui_tables.cpp
        imgui-docking/imgui_widgets.cpp
        imgui-docking/backends/imgui_impl_sdl2.cpp
        imgui-docking/backends/imgui_impl_sdlrenderer.cpp

        imgui-docking/imgui_demo.cpp # Used for demoing ImGUI
)

target_include_directories(Client PRIVATE imgui-docking)
target_link_libraries(Client ${SDL2_LIBRARIES})
