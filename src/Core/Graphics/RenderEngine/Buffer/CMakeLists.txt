target_sources(Graphics PRIVATE
    Buffer.cpp
    BufferBuilder.cpp
)

target_include_directories(Graphics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)