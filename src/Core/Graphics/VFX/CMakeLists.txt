target_sources(Graphics PRIVATE
    VFXWorld.cpp
    Effect.cpp
    ParticleHandleAllocator.cpp
)

target_include_directories(Graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)