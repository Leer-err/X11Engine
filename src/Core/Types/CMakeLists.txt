add_library(Types STATIC)

target_sources(Types PRIVATE
    Transform.cpp
)

target_include_directories(Types PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Types
    PRIVATE
        physx-lib
        glm::glm
)

target_compile_definitions(Types PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_ENABLE_EXPERIMENTAL GLM_FORCE_LEFT_HANDED)

add_subdirectory(Result)
add_subdirectory(Math)
add_subdirectory(Mesh)
add_subdirectory(Vertex)
