project(cimm)
cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++14")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(GoogleMock_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/libraries/GoogleMock/include")

add_subdirectory("source")
add_subdirectory("libraries")
