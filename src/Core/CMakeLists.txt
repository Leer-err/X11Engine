add_library(Core STATIC)

target_sources(Core PRIVATE
	Core.cpp
)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Core PUBLIC 
	Config
    Input
	Logger
	Window
	Graphics
	Types
	Utility
	File
	ECS
	Event
	Script
	# Physics
	Engine
)

add_subdirectory(Logger)
add_subdirectory(Types)
add_subdirectory(Input)
add_subdirectory(Config)
add_subdirectory(Window)
add_subdirectory(Graphics)
add_subdirectory(Utility)
add_subdirectory(File)
add_subdirectory(ECS)
add_subdirectory(Event)
# add_subdirectory(Physics)
add_subdirectory(Script)
add_subdirectory(Engine)