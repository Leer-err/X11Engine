target_sources(${TARGET_NAME} PRIVATE
    Scene.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Sky)
add_subdirectory(Camera)
add_subdirectory(Scripts)
add_subdirectory(GameInputContext)