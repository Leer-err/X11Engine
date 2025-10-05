cmake_minimum_required(VERSION 3.21)

include(FetchContent)

FetchContent_Declare(json 
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(${TARGET_NAME} PRIVATE
    nlohmann_json::nlohmann_json
)

target_sources(${TARGET_NAME} PRIVATE
    JSONKeyboardConfigReader.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)