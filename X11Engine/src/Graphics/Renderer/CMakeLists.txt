cmake_minimum_required(VERSION 3.21)

target_sources(${TARGET_NAME} PRIVATE
    Renderer.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${TARGET_NAME} PRIVATE
    d3d11.lib dxgi.lib
)

add_subdirectory(Context)
add_subdirectory(Rasterizer)
add_subdirectory(ResourceFactory)
add_subdirectory(SwapChain)
add_subdirectory(Shader)
add_subdirectory(Texture)
add_subdirectory(Buffer)
add_subdirectory(RenderTarget)
add_subdirectory(DepthStencil)
add_subdirectory(APIResources)
add_subdirectory(Resource)