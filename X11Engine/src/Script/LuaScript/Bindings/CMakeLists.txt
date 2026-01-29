

target_sources(${TARGET_NAME} PRIVATE
    InputBindings.cpp
    Bindings.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(ECSBindings)
add_subdirectory(TypeBindings)
add_subdirectory(TextureBindings)