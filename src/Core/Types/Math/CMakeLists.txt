

target_sources(Types PRIVATE
    Matrix.cpp
    Quaternion.cpp
    Vector3.cpp
    Vector4.cpp
)

target_include_directories(Types PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)