cmake_minimum_required(VERSION 3.21)

include(FetchContent)

FetchContent_Declare(DXmath
    GIT_REPOSITORY    https://github.com/microsoft/DirectXMath.git
    GIT_TAG apr2025
)
FetchContent_MakeAvailable(DXmath)

target_sources(${TARGET_NAME} PRIVATE
    Matrix.cpp
    Quaternion.cpp
    Transform.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${dxmath_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Result)
