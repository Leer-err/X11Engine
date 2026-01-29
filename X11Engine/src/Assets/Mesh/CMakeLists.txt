

target_sources(${TARGET_NAME} PRIVATE
    Mesh.cpp
    MeshBuilder.cpp
    MeshRegistry.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)