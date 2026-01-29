

target_sources(${TARGET_NAME} PRIVATE
    EntityBindings.cpp
    ECSBindings.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(ComponentBindings)