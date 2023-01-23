cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(imgui 
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp)

