add_library(Config STATIC)

target_sources(Config PRIVATE
    AppConfig.cpp
)

target_include_directories(Config PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(Window)
add_subdirectory(Graphics)