cmake_minimum_required(VERSION 3.15)

project(crashdown)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

add_executable(testbed
    main.cpp
)

set(OpenCV_DIR "/home/zz/soft/opencv-4.5/lib/cmake/opencv4" CACHE PATH "")
find_package(OpenCV REQUIRED)
target_link_libraries(testbed ${OpenCV_LIBS})
