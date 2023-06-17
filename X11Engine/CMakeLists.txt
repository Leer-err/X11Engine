cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

include(FetchContent)

FetchContent_Declare(assimp
  GIT_REPOSITORY    https://github.com/assimp/assimp.git
  GIT_TAG           c8dafe0d2887242285c0080c6cbbea8c1f1c8094
)
FetchContent_MakeAvailable(assimp)

FetchContent_Declare(directxtk
  GIT_REPOSITORY    https://github.com/microsoft/DirectXTK.git
  GIT_TAG           2c9dc8930d41f05bb7b04e09b7824e43b6cea08d
)
FetchContent_MakeAvailable(directxtk)

FetchContent_Declare(imgui
  GIT_REPOSITORY    https://github.com/ocornut/imgui.git
  GIT_TAG           v1.89.3
)
FetchContent_MakeAvailable(imgui)
file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp 
	${imgui_SOURCE_DIR}/backends/imgui_impl_DX11.cpp 
	${imgui_SOURCE_DIR}/backends/imgui_impl_Win32.cpp)
file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h
	${imgui_SOURCE_DIR}/backends/imgui_impl_DX11.h 
	${imgui_SOURCE_DIR}/backends/imgui_impl_Win32.h)
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PRIVATE ${imgui_SOURCE_DIR})

FetchContent_Declare(implot
  GIT_REPOSITORY    https://github.com/epezent/implot.git
  GIT_TAG           v0.14
)
FetchContent_MakeAvailable(implot)
file(GLOB IMPLOT_SOURCES ${implot_SOURCE_DIR}/*.cpp)
file(GLOB IMPLOT_HEADERS ${implot_SOURCE_DIR}/*.h)
list(REMOVE_ITEM IMPLOT_SOURCES implot_demo.cpp)
add_library(implot STATIC ${IMPLOT_SOURCES} ${IMPLOT_HEADERS})
target_include_directories(implot PRIVATE ${implot_SOURCE_DIR} ${imgui_SOURCE_DIR})

FetchContent_Declare(p7
  URL    http://baical.net/files/libP7Client_v5.6.zip
)
FetchContent_MakeAvailable(p7)

FetchContent_Declare(json 
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(stb 
  URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(stb)

add_executable(Engine WIN32 ${SOURCES} ${HEADERS})

target_include_directories(Engine PRIVATE
	${PATH_P7_API}
	${CMAKE_CURRENT_SOURCE_DIR}
	${DirectXTK_SOURCE_DIR}/Inc
	${stb_SOURCE_DIR}
	${imgui_SOURCE_DIR}
	${implot_SOURCE_DIR}
)

add_compile_definitions(NOMINMAX)
add_compile_definitions(WIN32_LEAN_AND_MEAN)

target_link_libraries(Engine PRIVATE
	D3DCompiler
    d3d11
	dxgi
	DirectXTK
	assimp
	p7
	nlohmann_json::nlohmann_json
	imgui
	implot
)

set_target_properties(Engine PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED true
)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/assimp-vc143-mt.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
		${ASSIMP_DLL}
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assimp-vc143-mt.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Shaders
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Shaders)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Assets
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Assets)

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/assimp-vc143-mt.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
		${ASSIMP_DLL}
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assimp-vc143-mt.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Shaders
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Shaders)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Assets
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Assets)

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/assimp-vc143-mtd.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
		${ASSIMP_DLL}
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assimp-vc143-mtd.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Shaders
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Shaders)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Assets
		${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Assets)
endif()

