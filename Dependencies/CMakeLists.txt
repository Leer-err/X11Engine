cmake_minimum_required (VERSION 3.21)

include(FetchContent)
include(ExternalProject)

FetchContent_Declare(
	tracy
	GIT_REPOSITORY https://github.com/wolfpld/tracy.git
	GIT_TAG v0.12.2
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tracy)

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

add_library(physx-lib INTERFACE)

set(PHYSX_BUILD_TYPE $<IF:$<CONFIG:Debug>,checked,profile>)

target_include_directories(physx-lib INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/include
)

target_link_directories(physx-lib INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/${PHYSX_BUILD_TYPE}
	${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/linux.x86_64/${PHYSX_BUILD_TYPE}
)

target_link_libraries(physx-lib INTERFACE
	PhysXExtensions_static_64
	PhysXPvdSDK_static_64
)

if (WIN32)
 	target_link_libraries(physx-lib INTERFACE
    	PhysX_64
		PhysXCommon_64
		PhysXFoundation_64
 	)
else()
  	target_link_libraries(physx-lib INTERFACE
    	PhysX_static_64
		PhysXCommon_static_64
		PhysXFoundation_static_64
	)
endif()

# add_custom_command(
# 	OUTPUT
#         ${CMAKE_BINARY_DIR}/bin/PhysX_64.dll
# 		${CMAKE_BINARY_DIR}/bin/PhysXFoundation_64.dll
# 		${CMAKE_BINARY_DIR}/bin/PhysXCommon_64.dll
#     COMMAND 
# 		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/${PHYSX_BUILD_TYPE}/PhysX_64.dll ${CMAKE_BINARY_DIR}/bin
# 	COMMAND
# 		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/${PHYSX_BUILD_TYPE}/PhysXFoundation_64.dll ${CMAKE_BINARY_DIR}/bin
# 	COMMAND
# 		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/${PHYSX_BUILD_TYPE}/PhysXCommon_64.dll ${CMAKE_BINARY_DIR}/bin
#     DEPENDS ${EXECUTABLE_NAME}
# )
# add_custom_target(copy_physx_libs ALL
#     DEPENDS ${CMAKE_BINARY_DIR}/bin/PhysX_64.dll
# 	DEPENDS ${CMAKE_BINARY_DIR}/bin/PhysXFoundation_64.dll
# )

# Assimp

add_library(Assimp INTERFACE)

target_include_directories(Assimp INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/assimp/include
)

target_link_directories(Assimp INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/assimp/lib/Debug
	${CMAKE_CURRENT_SOURCE_DIR}/assimp/lib/Release
	${CMAKE_CURRENT_SOURCE_DIR}/assimp/bin
)

if (WIN32)
	set(ASSIMP_LIB_FILE $<IF:$<CONFIG:Debug>,assimp-vc143-mtd.dll,assimp-vc143-mt.dll>)

	add_custom_command(
		OUTPUT
	        ${CMAKE_BINARY_DIR}/bin/${ASSIMP_LIB_FILE}
		COMMAND
			${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/assimp/bin/${CMAKE_BUILD_TYPE}/${ASSIMP_LIB_FILE} ${CMAKE_BINARY_DIR}/bin
	    DEPENDS ${EXECUTABLE_NAME}
	)
else()
  	set(ASSIMP_LIB_FILE libassimp.so)

	add_custom_command(
		OUTPUT
	        ${CMAKE_BINARY_DIR}/bin/${ASSIMP_LIB_FILE}
		COMMAND
			${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/assimp/bin/${ASSIMP_LIB_FILE} ${CMAKE_BINARY_DIR}/bin
	    DEPENDS ${EXECUTABLE_NAME}
	)
endif()

target_link_libraries(Assimp 
	INTERFACE
		${ASSIMP_LIB_FILE}
)


add_custom_target(copy_assimp_libs ALL
	DEPENDS ${CMAKE_BINARY_DIR}/bin/${ASSIMP_LIB_FILE}
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
)

target_sources(ImGuiLib PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)

target_link_libraries(ImGuiLib PRIVATE
	SDL3::SDL3
	vulkan-interface
)

#Lua

# target_include_directories(${TARGET_NAME} PUBLIC
# 	${CMAKE_CURRENT_SOURCE_DIR}/lua/include
# )

# target_link_directories(${TARGET_NAME} PUBLIC
# 	${CMAKE_CURRENT_SOURCE_DIR}/lua
# )

# find_library(LUA_PATH PATH liblua54.a HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lua/)

# target_link_libraries(${TARGET_NAME} PUBLIC 
# 	${LUA_PATH}
# )

# add_custom_command(
# 	OUTPUT
#         ${CMAKE_BINARY_DIR}/bin/lua54.dll
# 	COMMAND
# 		${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lua/lua54.dll ${CMAKE_BINARY_DIR}/bin
#     DEPENDS ${EXECUTABLE_NAME}
# )
# add_custom_target(copy_lua_libs ALL
# 	DEPENDS ${CMAKE_BINARY_DIR}/bin/lua54.dll
# )

# glm

add_subdirectory(glm EXCLUDE_FROM_ALL)

# SDL3

add_subdirectory(sdl EXCLUDE_FROM_ALL)

# vk-bootstrap

add_subdirectory(vk-bootstrap)

# Vulkan

add_library(vulkan-interface INTERFACE)

if (WIN32)
 	target_include_directories(vulkan-interface INTERFACE
		${VULKAN_PATH}/include
	)
	target_link_directories(vulkan-interface INTERFACE
		${VULKAN_PATH}/Lib
	)
	target_link_libraries(vulkan-interface INTERFACE vulkan-1)
else()
	find_package(Vulkan REQUIRED)
  	target_link_libraries(vulkan-interface INTERFACE Vulkan::Vulkan)
endif()

# Vulkan Memory Allocator

add_subdirectory(VulkanMemoryAllocator)

add_library(vulkan-allocator STATIC)
target_sources(vulkan-allocator 
	PRIVATE
		VMAUsage.cpp
)
target_link_libraries(vulkan-allocator 
	PUBLIC 
		GPUOpen::VulkanMemoryAllocator
	PRIVATE
		vulkan-interface
)

# SPIRV-Reflect

add_library(SPIRV-Reflect STATIC)
target_sources(SPIRV-Reflect 
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Reflect/spirv_reflect.c
)
target_include_directories(SPIRV-Reflect PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Reflect/
)

# STB

add_library(STB STATIC)

target_include_directories(STB PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/stb/
)

target_sources(STB
	PRIVATE
		STBUsage.cpp
)

# meshoptimizer

add_library(meshoptimizer STATIC)

target_include_directories(meshoptimizer PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/meshoptimizer/src
)

target_sources(meshoptimizer
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/meshoptimizer/src/meshletutils.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/meshoptimizer/src/clusterizer.cpp
)