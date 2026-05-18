add_library(ECS STATIC)

target_link_libraries(ECS
    PUBLIC
        Script
    PRIVATE
        Utility
        Tracy::TracyClient
)

add_subdirectory(Component)
add_subdirectory(Entity)
add_subdirectory(System)
add_subdirectory(Query)
add_subdirectory(World)