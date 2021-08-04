cmake_minimum_required(VERSION 3.17)

project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)



add_executable(testbed main.cpp)
# target_include_directories(testbed PRIVATE "/home/zz/work/git_mirror/fmt/include")


find_package(OpenMP)
if(NOT TARGET OpenMP::OpenMP_CXX AND (OpenMP_CXX_FOUND OR OPENMP_FOUND))
    target_compile_options(testbed PRIVATE ${OpenMP_CXX_FLAGS})
endif()

if(OpenMP_CXX_FOUND OR OPENMP_FOUND)
    if(ANDROID_NDK_MAJOR AND (ANDROID_NDK_MAJOR GREATER 20))
        target_compile_options(testbed PRIVATE -fopenmp)
        target_link_libraries(testbed PUBLIC -fopenmp -static-openmp)
    elseif(OpenMP_CXX_FOUND)
        target_link_libraries(testbed PUBLIC OpenMP::OpenMP_CXX)
    else()
        target_link_libraries(testbed PRIVATE "${OpenMP_CXX_FLAGS}")
    endif()
endif()