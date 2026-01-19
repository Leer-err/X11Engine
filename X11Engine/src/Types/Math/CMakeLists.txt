cmake_minimum_required(VERSION 3.21)

target_sources(${TARGET_NAME} PRIVATE
    Matrix.cpp
    Quaternion.cpp
    Vector3.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)