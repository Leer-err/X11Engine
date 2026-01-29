

target_sources(${TARGET_NAME} PRIVATE
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(PixelShader)
add_subdirectory(VertexShader)
add_subdirectory(ShaderSource)