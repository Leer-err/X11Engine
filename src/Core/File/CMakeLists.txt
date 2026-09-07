add_library(File STATIC)

target_sources(File PRIVATE
)

target_include_directories(File PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(File
    PUBLIC
        Config
        Types
        Assimp
        nlohmann_json::nlohmann_json
        meshoptimizer
)

add_subdirectory(ModelReader)
add_subdirectory(ConfigSerializer)
