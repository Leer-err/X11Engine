cmake_minimum_required (VERSION 3.21)

include(FetchContent)
include(ExternalProject)

FetchContent_Declare(
	tracy
	GIT_REPOSITORY https://github.com/wolfpld/tracy.git
	GIT_TAG master
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tracy)

target_link_libraries(${TARGET_NAME} PRIVATE
	TracyClient
)

# add_custom_command(
# 	OUTPUT
#         ${CMAKE_BINARY_DIR}/bin/TracyClientd.dll
#     COMMAND 
# 		${CMAKE_COMMAND} -E copy ${tracy_BINARY_DIR}/TracyClientd.dll ${CMAKE_BINARY_DIR}/bin
#     DEPENDS ${EXECUTABLE_NAME}
# )
# add_custom_target(copy_tracy_libs ALL
#     DEPENDS ${CMAKE_BINARY_DIR}/bin/TracyClientd.dll
# )

#PhysX

target_include_directories(${TARGET_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/include
)

target_link_directories(${TARGET_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug
)

target_link_libraries(${TARGET_NAME} PRIVATE
    PhysX_64
	PhysXCommon_64
	PhysXFoundation_64
	PhysXExtensions_static_64
	PhysXPvdSDK_static_64
)

add_custom_command(
	OUTPUT
        ${CMAKE_BINARY_DIR}/bin/PhysX_64.dll
		${CMAKE_BINARY_DIR}/bin/PhysXFoundation_64.dll
		${CMAKE_BINARY_DIR}/bin/PhysXCommon_64.dll
    COMMAND 
		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug/PhysX_64.dll ${CMAKE_BINARY_DIR}/bin
	COMMAND
		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug/PhysXFoundation_64.dll ${CMAKE_BINARY_DIR}/bin
	COMMAND
		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug/PhysXCommon_64.dll ${CMAKE_BINARY_DIR}/bin
    DEPENDS ${EXECUTABLE_NAME}
)
add_custom_target(copy_physx_libs ALL
    DEPENDS ${CMAKE_BINARY_DIR}/bin/PhysX_64.dll
	DEPENDS ${CMAKE_BINARY_DIR}/bin/PhysXFoundation_64.dll
)

#Assimp

target_include_directories(${TARGET_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/assimp/include
)

target_link_directories(${TARGET_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/assimp/lib/Debug
)

target_link_libraries(${TARGET_NAME} PUBLIC
    assimp-vc143-mtd
)

add_custom_command(
	OUTPUT
        ${CMAKE_BINARY_DIR}/bin/assimp-vc143-mtd.dll
	COMMAND
		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/assimp/bin/Debug/assimp-vc143-mtd.dll ${CMAKE_BINARY_DIR}/bin
    DEPENDS ${EXECUTABLE_NAME}
)
add_custom_target(copy_assimp_libs ALL
	DEPENDS ${CMAKE_BINARY_DIR}/bin/assimp-vc143-mtd.dll
)

# ImGui

add_library(ImGuiLib STATIC)

target_include_directories(ImGuiLib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/
)

target_sources(ImGuiLib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_win32.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_dx11.cpp
)

target_link_libraries(${TARGET_NAME} PUBLIC
    ImGuiLib
)