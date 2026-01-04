cmake_minimum_required (VERSION 3.21)

set(EXECUTABLE_NAME "X11Engine")

add_executable(${EXECUTABLE_NAME} WIN32 main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE X11Engine_lib)
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED true
)

set(TARGET_NAME "X11Engine_lib")

add_library(${TARGET_NAME} STATIC)

target_link_libraries(${TARGET_NAME} PUBLIC TracyClient)

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${TARGET_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED true
)

add_subdirectory(Dependencies)

add_subdirectory(Input)
add_subdirectory(Event)
add_subdirectory(FileReader)
add_subdirectory(Graphics)
add_subdirectory(Window)
add_subdirectory(Engine)
add_subdirectory(Logger)
add_subdirectory(Types)
add_subdirectory(Camera)
add_subdirectory(ECS)
# add_subdirectory(Systems)
add_subdirectory(Components)
add_subdirectory(Utility)
add_subdirectory(Assets)
add_subdirectory(Physics)
# add_subdirectory(Script)
add_subdirectory(Config)
add_subdirectory(Scene)
add_subdirectory(Overlay)