add_library(Graphics STATIC)

target_sources(Graphics PRIVATE
    Graphics.cpp
)

target_include_directories(Graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Graphics
    PRIVATE 
        Window
        Logger
        Types
        Utility
        Tracy::TracyClient
        SDL3::SDL3
        ImGuiLib
        vulkan-interface
        vulkan-allocator
        vk-bootstrap::vk-bootstrap
        SPIRV-Reflect
)

add_subdirectory(RenderEngine)
add_subdirectory(RenderWorld)
add_subdirectory(Overlay)
add_subdirectory(Renderers)
add_subdirectory(RenderData)
# add_subdirectory(GraphicsCommunicationManager)