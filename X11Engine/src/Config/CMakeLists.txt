cmake_minimum_required(VERSION 3.21)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Window)
add_subdirectory(Graphics)