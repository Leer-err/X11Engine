target_sources(ECS PRIVATE
)

target_include_directories(ECS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#add_subdirectory(PositionComponents)
# add_subdirectory(PlayerComponents)
# add_subdirectory(PhysicsComponents)
add_subdirectory(GeneralComponents)
# add_subdirectory(ScriptComponent)
