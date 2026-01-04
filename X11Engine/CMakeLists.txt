cmake_minimum_required(VERSION 3.21)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(src)
# add_subdirectory(test)

add_custom_command(
	OUTPUT
        ${CMAKE_BINARY_DIR}/bin/Assets/
	COMMAND
		${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets/ ${CMAKE_BINARY_DIR}/bin/Assets/
    DEPENDS ${EXECUTABLE_NAME}
)
add_custom_target(copy_assets ALL
	DEPENDS ${CMAKE_BINARY_DIR}/bin/Assets/
)