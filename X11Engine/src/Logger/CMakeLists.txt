

include(FetchContent)

FetchContent_Declare(spdlog 
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(spdlog)

target_link_libraries(${TARGET_NAME} PRIVATE
    spdlog::spdlog_header_only
)

target_sources(${TARGET_NAME} PRIVATE
    Logger.cpp
    LoggerFactory.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${spdlog_SOURCE_DIR}/include
)