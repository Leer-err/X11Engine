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


if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/assimp-vc143-mt.dll)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/Debug/assimp-vc143-mtd.dll)
endif()

add_executable(Engine WIN32 ${SOURCES} ${HEADERS})

target_include_directories(Engine PRIVATE
	${PATH_P7_API}
	${CMAKE_CURRENT_SOURCE_DIR}
	${DirectXTK_SOURCE_DIR}/Inc
	${stb_SOURCE_DIR}
)

add_compile_definitions(NOMINMAX)
add_compile_definitions(WIN32_LEAN_AND_MEAN)

target_link_libraries(Engine PRIVATE
	D3DCompiler.lib
    d3d11.lib
	dxgi.lib
	dxguid.lib
	uuid.lib
    kernel32.lib
	user32.lib
    comdlg32.lib
	advapi32.lib
	shell32.lib
    ole32.lib
	oleaut32.lib
	DirectXTK
	assimp
	p7
	nlohmann_json::nlohmann_json
)

set_target_properties(Engine PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED true
)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
		${ASSIMP_DLL}
		${CMAKE_CURRENT_BINARY_DIR}/Debug/assimp-vc143-mtd.dll)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Shaders
		${CMAKE_CURRENT_BINARY_DIR}/Debug/Shaders)

add_custom_command(
        TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/Assets
		${CMAKE_CURRENT_BINARY_DIR}/Debug/Assets)