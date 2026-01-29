target_sources(${TARGET_NAME} PRIVATE
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Resources)
add_subdirectory(Shape)
add_subdirectory(Actor)
add_subdirectory(RigidBody)
add_subdirectory(Scene)