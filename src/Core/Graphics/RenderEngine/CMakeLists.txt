target_sources(Graphics PRIVATE
    RenderEngine.cpp
    RenderingBackend.cpp
)

target_include_directories(Graphics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Device)
add_subdirectory(SwapChain)
add_subdirectory(Mesh)
add_subdirectory(RenderEnviroment)
add_subdirectory(DeviceProperties)
add_subdirectory(DescriptorSet)
add_subdirectory(ShaderRegistry)
add_subdirectory(TextureRegistry)
add_subdirectory(Texture)
add_subdirectory(Utility)
add_subdirectory(AdapterTypes)
add_subdirectory(StagingBuffer)
# add_subdirectory(BufferedUniform)
add_subdirectory(BufferRegistry)
add_subdirectory(Buffer)