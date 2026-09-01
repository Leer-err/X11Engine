add_library(EssentialsLoader STATIC)

target_link_libraries(EssentialsLoader
    PRIVATE
        File
        Logger
        Graphics
)

target_sources(EssentialsLoader PRIVATE
    EssentialsLoader.cpp
)

target_include_directories(EssentialsLoader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)