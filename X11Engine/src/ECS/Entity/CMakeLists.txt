

target_sources(${TARGET_NAME} PRIVATE
    EntityRegistry.cpp
    Entity.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)