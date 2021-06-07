cmake_minimum_required(VERSION 3.15)

project(test_mat_multithread)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")


set(ncnn_DIR "/home/zz/work/ncnn/build/install/lib/cmake/ncnn" CACHE PATH "")
find_package(ncnn REQUIRED)

set(OpenCV_DIR "/home/zz/lib/opencv/4.5.2/lib/cmake/opencv4" CACHE PATH "")
find_package(OpenCV REQUIRED)

add_executable(ncnn_test ncnn_test.cpp)
target_link_libraries(ncnn_test ncnn pthread)

add_executable(opencv_test opencv_test.cpp)
target_link_libraries(opencv_test ${OpenCV_LIBS} pthread)

