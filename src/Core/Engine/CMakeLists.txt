add_library(Engine STATIC)

target_sources(Engine PRIVATE
    Engine.cpp
)

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Engine 
    PUBLIC
        Graphics
        Utility
        Types
        Input
        Window
        STB
        Tracy::TracyClient
)