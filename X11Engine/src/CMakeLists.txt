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
add_subdirectory(FileReader)
add_subdirectory(Graphics)
add_subdirectory(Window)
add_subdirectory(Engine)
add_subdirectory(Logger)
add_subdirectory(Types)
add_subdirectory(Camera)
add_subdirectory(ECS)
add_subdirectory(Systems)
add_subdirectory(Components)
add_subdirectory(Utility)
add_subdirectory(Assets)
add_subdirectory(Physics)

# file(GLOB_RECURSE SOURCES *.cpp)
# file(GLOB_RECURSE HEADERS *.h)

# include(FetchContent)

# FetchContent_Declare(assimp
#   GIT_REPOSITORY    https://github.com/assimp/assimp.git
# )
# FetchContent_MakeAvailable(assimp)

# FetchContent_Declare(imgui
#   GIT_REPOSITORY    https://github.com/ocornut/imgui.git
# )
# FetchContent_MakeAvailable(imgui)
# file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp 
# 	${imgui_SOURCE_DIR}/backends/imgui_impl_DX11.cpp 
# 	${imgui_SOURCE_DIR}/backends/imgui_impl_Win32.cpp)
# file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h
# 	${imgui_SOURCE_DIR}/backends/imgui_impl_DX11.h 
# 	${imgui_SOURCE_DIR}/backends/imgui_impl_Win32.h)
# add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
# target_include_directories(imgui PRIVATE ${imgui_SOURCE_DIR})

# FetchContent_Declare(implot
#   GIT_REPOSITORY    https://github.com/epezent/implot.git
# )
# FetchContent_MakeAvailable(implot)
# file(GLOB IMPLOT_SOURCES ${implot_SOURCE_DIR}/*.cpp)
# file(GLOB IMPLOT_SOURCES ${implot_SOURCE_DIR}/*.cpp)
# file(GLOB IMPLOT_HEADERS ${implot_SOURCE_DIR}/*.h)
# list(REMOVE_ITEM IMPLOT_SOURCES implot_demo.cpp)
# add_library(implot STATIC ${IMPLOT_SOURCES} ${IMPLOT_HEADERS})
# target_include_directories(implot PRIVATE ${implot_SOURCE_DIR} ${imgui_SOURCE_DIR})

# FetchContent_Declare(json 
#   URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
# )
# FetchContent_MakeAvailable(json)

# add_executable(Engine WIN32 main.cpp Framework/FileReader/WICImageReader.cpp Framework/Graphics/Image.cpp)#${SOURCES} ${HEADERS})

# target_include_directories(Engine PRIVATE
# 	${CMAKE_CURRENT_SOURCE_DIR}
# 	${DirectXTK_SOURCE_DIR}/Inc
# 	${imgui_SOURCE_DIR}
# 	${implot_SOURCE_DIR}
# 	${assimp_SOURCE_DIR}/include/assimp
# )

# add_compile_definitions(NOMINMAX)
# add_compile_definitions(WIN32_LEAN_AND_MEAN)

# target_link_libraries(Engine PRIVATE
# 	D3DCompiler
#     d3d11
# 	dxgi
# 	assimp
# 	nlohmann_json::nlohmann_json
# 	imgui
# 	implot
# )

# set_target_properties(Engine PROPERTIES
# 	CXX_STANDARD 17
# 	CXX_STANDARD_REQUIRED true
# )

# if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR "Release")

# set(ASSIMP_DLL assimp-vc143-mt.dll)

# elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# set(ASSIMP_DLL assimp-vc143-mtd.dll)

# endif()

# # add_custom_command(
# #         TARGET Engine POST_BUILD
# #         COMMAND ${CMAKE_COMMAND} -E copy
# # 		${assimp_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${ASSIMP_DLL}
# # 		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${ASSIMP_DLL})

# add_custom_command(
#         TARGET Engine POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
# 		${CMAKE_CURRENT_SOURCE_DIR}/Shaders
# 		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Shaders)

# add_custom_command(
#         TARGET Engine POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
# 		${CMAKE_CURRENT_SOURCE_DIR}/Assets
# 		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Assets)

