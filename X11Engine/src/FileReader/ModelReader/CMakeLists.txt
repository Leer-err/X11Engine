cmake_minimum_required(VERSION 3.21)

target_sources(${TARGET_NAME} PRIVATE
    AssimpModelReader.cpp
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add_custom_command(OUTPUT
#         ${CMAKE_BINARY_DIR}/bin/assimpd.dll
#     COMMAND ${CMAKE_COMMAND} -E copy ${assimp_BINARY_DIR}/bin/assimpd.dll
#     ${CMAKE_BINARY_DIR}/bin
#     DEPENDS assimp
# )
# add_custom_target(copy_files ALL
#     DEPENDS ${CMAKE_BINARY_DIR}/bin/assimpd.dll
# )
