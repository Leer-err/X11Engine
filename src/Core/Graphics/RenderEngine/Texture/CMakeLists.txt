target_sources(Graphics PRIVATE
    Texture.cpp
    TextureBuilder.cpp
)

target_include_directories(Graphics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)