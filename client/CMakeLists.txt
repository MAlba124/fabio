cmake_minimum_required(VERSION 3.24)
project(client)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIRS})

add_compile_options(-lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp)
add_executable(client poker.cpp)





add_subdirectory(imgui)
list(APPEND EXTRA_LIBS imgui)




add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/include/glad.c)
list(APPEND EXTRA_LIBS glad)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/)


target_link_libraries(client PUBLIC ${EXTRA_LIBS} glfw ${ASSIMP_LIBRARIES})

