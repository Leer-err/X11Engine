

target_sources(${TARGET_NAME} PUBLIC
    Sampler.cpp
    SamplerBuilder.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(CommonSamplers)