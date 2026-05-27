target_sources(Graphics PRIVATE
    PostProcessingPass.cpp
)

target_include_directories(Graphics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(DitheringPass)