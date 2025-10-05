cmake_minimum_required(VERSION 3.21)

target_sources(${TARGET_NAME} PRIVATE
    D3DShaderBlob.cpp
)

target_link_libraries(${TARGET_NAME} PRIVATE
    d3dcompiler
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)