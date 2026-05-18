cmake_minimum_required(VERSION 3.20)

project(
    labwork10
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.16.0/googletest-1.16.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    FIND_PACKAGE_ARGS NAMES GTest
)

FetchContent_MakeAvailable(googletest)

add_library(db_lib)

add_subdirectory(lib/src)
add_subdirectory(lib/src/string)
add_subdirectory(lib/src/list)
add_subdirectory(lib/src/set)
add_subdirectory(lib/src/geo)
add_subdirectory(lib/src/general)
add_subdirectory(lib/utility)

target_include_directories(db_lib
    PUBLIC
    ${PROJECT_SOURCE_DIR}/lib/include
)

target_compile_features(db_lib PUBLIC cxx_std_23)

add_executable(labwork10
    bin/main.cpp
)

target_link_libraries(labwork10
    PRIVATE db_lib
)

enable_testing()
add_executable(
    database-lib-tests
    tests/tests.cpp
)

target_link_libraries(database-lib-tests PRIVATE db_lib gtest_main gmock)

include(GoogleTest)
gtest_discover_tests(database-lib-tests)
