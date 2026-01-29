

target_sources(${TARGET_NAME} PRIVATE
    ImageReaderFactory.cpp
    WICImageReader.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(STBImageReader)