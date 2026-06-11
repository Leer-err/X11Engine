target_sources(Graphics PRIVATE
    Effect.cpp
    Emitter.cpp
    ParticlePool.cpp
    VFXWorld.cpp
)

target_include_directories(Graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)