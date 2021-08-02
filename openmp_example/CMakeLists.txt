cmake_minimum_required(VERSION 3.17)

project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)



add_executable(testbed main.cpp)

set(OpenMP_COMPILE_OPTIONS "")

find_package(OpenMP REQUIRED)
if(NOT TARGET OpenMP::OpenMP_CXX AND (OpenMP_CXX_FOUND OR OPENMP_FOUND))
    target_compile_options(testbed PRIVATE ${OpenMP_CXX_FLAGS})
    #list(APPEND OpenMP_COMPILE_OPTIONS ${OpenMP_CXX_FLAGS})
endif()


if(OpenMP_CXX_FOUND)
    target_link_libraries(testbed PUBLIC OpenMP::OpenMP_CXX)
endif()

# if(OpenMP_CXX_FOUND OR OPENMP_FOUND)
#     if(ANDROID_NDK_MAJOR AND (ANDROID_NDK_MAJOR GREATER 20))
#         target_compile_options(testbed PRIVATE -fopenmp)
#         #list(APPEND OpenMP_COMPILE_OPTIONS -fopenmp)
#         target_link_libraries(testbed PUBLIC -fopenmp -static-openmp)
#         #set(OpenMP_LIBS PUBLIC -fopenmp -static-openmp)
#     elseif(OpenMP_CXX_FOUND)
#         target_link_libraries(testbed PUBLIC OpenMP::OpenMP_CXX)
#         #set(OpenMP_LIBS OpenMP::OpenMP_CXX)
#     else()
#         target_link_libraries(testbed PRIVATE "${OpenMP_CXX_FLAGS}")
#         #set(OpenMP_LIBS ${OpenMP_CXX_FLAGS})
#     endif()
# endif()

