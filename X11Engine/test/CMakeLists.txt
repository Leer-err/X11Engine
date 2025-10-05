cmake_minimum_required(VERSION 3.21)

include(FetchContent)
include(ctest)

FetchContent_Declare(test
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG v1.16.0
  EXCLUDE_FROM_ALL
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(test)

set(TEST_NAME "X11Engine_tests")

file(GLOB_RECURSE SOURCES *.c *.cpp)

add_executable(${TEST_NAME} ${SOURCES})

add_compile_definitions(
	GTEST_LINKED_AS_SHARED_LIBRARY
)

target_link_libraries(${TEST_NAME} PRIVATE
	X11Engine_lib
	GTest::gmock
	GTest::gtest
)

set_target_properties(${TEST_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED true
)

add_test(
    NAME runTests
    COMMAND ${TEST_NAME}
)