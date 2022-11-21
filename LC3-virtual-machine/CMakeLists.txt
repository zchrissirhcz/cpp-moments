cmake_minimum_required(VERSION 3.20)
project(lc3-virtual-machine)

set(CMAKE_C_STANDARD 99)

include("overlook.cmake")
#include("asan.cmake")

add_executable(testbed
  main.c
)
