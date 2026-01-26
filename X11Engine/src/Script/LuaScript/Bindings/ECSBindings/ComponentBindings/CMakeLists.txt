cmake_minimum_required(VERSION 3.21)

target_sources(${TARGET_NAME} PRIVATE
    ComponentMetatables.cpp
    TransformBindings.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(MeshBindings)
add_subdirectory(StaticMaterialBindings)