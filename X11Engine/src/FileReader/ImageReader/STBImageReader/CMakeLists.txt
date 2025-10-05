cmake_minimum_required(VERSION 3.21)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)

target_sources(${TARGET_NAME} PRIVATE
    STBImageReader.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${stb_SOURCE_DIR}
)