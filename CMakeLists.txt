cmake_minimum_required(VERSION 3.29)
project(mtfind)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)

configure_file(input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt COPYONLY)