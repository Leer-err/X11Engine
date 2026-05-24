cmake_minimum_required (VERSION 3.21)

set(EXECUTABLE_NAME "X11Engine")

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE X11Engine_lib)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED true
)

set(TARGET_NAME "X11Engine_lib")

add_library(${TARGET_NAME} STATIC)

target_link_libraries(${TARGET_NAME} PUBLIC 
	TracyClient 
	Core
	STB
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${TARGET_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED true
	CXX_EXTENSIONS OFF
)

add_subdirectory(Camera)
# add_subdirectory(Script)
add_subdirectory(Scene)
add_subdirectory(Core)