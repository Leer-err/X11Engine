

target_sources(${TARGET_NAME} PRIVATE
    QueryBuilder.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)