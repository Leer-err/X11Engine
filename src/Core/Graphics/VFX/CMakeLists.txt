target_sources(Graphics PRIVATE
    ParticlePool.cpp
    VFXWorld.cpp
    Emitter.cpp
)

target_include_directories(Graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)