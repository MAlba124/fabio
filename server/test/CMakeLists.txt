cmake_minimum_required(VERSION 3.24)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
        -Wall -Wextra -Werror
        -O2
)

project(test)

add_executable(${PROJECT_NAME}_client
        test_client.cpp
)

add_executable(${PROJECT_NAME}_new
        new_test_client.cpp
)

target_link_libraries(${PROJECT_NAME}_new
        PUBLIC ${EXTRA_LIBS}
)