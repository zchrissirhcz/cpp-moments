cmake_minimum_required(VERSION 3.15)

project(gdb_segfault_vptr)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O0")

add_executable(testbed
    main.cpp
)

