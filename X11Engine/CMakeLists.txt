cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

include(FetchContent)

FetchContent_Declare(assimp
  GIT_REPOSITORY    https://github.com/assimp/assimp.git
  GIT_TAG           c8dafe0d2887242285c0080c6cbbea8c1f1c8094
)
FetchContent_MakeAvailable(assimp)
FetchContent_Declare(p7
  URL    http://baical.net/files/libP7Client_v5.6.zip
)
FetchContent_MakeAvailable(p7)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/assimp-vc143-mt.dll)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(ASSIMP_DLL ${assimp_BINARY_DIR}/bin/assimp-vc143-mtd.dll)
endif()

install(FILES ${ASSIMP_DLL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB ASSETS "models/*.*")
file(COPY ${ASSETS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/aseets)
configure_file("Graphics/PixelShader.hlsl" ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file("Graphics/VertexShader.hlsl" ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(Engine WIN32 ${SOURCES} ${HEADERS})

target_include_directories(Engine PRIVATE
	${PATH_P7_API}
	${CMAKE_CURRENT_SOURCE_DIR}
	ECS/Component
	ECS/Entity
	ECS/System
	Types
	Memory
)

target_link_libraries(Engine "d3d11" "dxgi" "d3dcompiler" assimp p7)

set_target_properties(Engine PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED true
)