cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.h)

configure_file("Graphics/PixelShader.hlsl" "" COPYONLY)
configure_file("Graphics/VertexShader.hlsl" "" COPYONLY)

add_executable(Engine WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(Engine "d3d11" "dxgi" "d3dcompiler")

set_target_properties(Engine PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED true
)