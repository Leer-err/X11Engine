cmake_minimum_required(VERSION 3.21)

target_sources(${TARGET_NAME} PRIVATE
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(LuaScript)