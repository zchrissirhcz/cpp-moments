cmake_minimum_required(VERSION 3.17)

project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)





#--- OpenCV
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        #set(OpenCV_DIR "/home/zz/soft/opencv-4.5.1/lib/cmake/opencv4" CACHE PATH "") # shared
        #set(OpenCV_DIR "/home/zz/lib/opencv/4.5.2/lib/cmake/opencv4" CACHE PATH "") # static
        #set(OpenCV_DIR "/home/zz/lib/opencv/4.5.4-pre-static-dbg/lib/cmake/opencv4" CACHE PATH "") # static
        set(OpenCV_DIR "/home/zz/lib/opencv/4.5.4-pre-static/lib/cmake/opencv4" CACHE PATH "")
        #set(OpenCV_DIR "/home/zz/lib/opencv/4.5.4-pre-static-mt/lib/cmake/opencv4" CACHE PATH "")
        #set(OpenCV_DIR "/home/zz/lib/opencv/4.5.4-pre-static-simd/lib/cmake/opencv4" CACHE PATH "") # static
    elseif (ANDROID)
        #set(OpenCV_DIR "/home/zz/lib/opencv-4.5.2-android-sdk/sdk/native/jni" CACHE PATH "")
        set(OpenCV_DIR "/home/zz/lib/opencv-4.5.4-pre-android-sdk/sdk/native/jni" CACHE PATH "")
        #set(OpenCV_DIR "/home/zz/lib/opencv-4.5.4-pre-android-sdk-mt/sdk/native/jni" CACHE PATH "")
    endif()
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(OpenCV_DIR "E:/lib/opencv/4.5.2" CACHE PATH "")
    elseif (ANDROID)
        set(OpenCV_DIR "E:/soft/Android/opencv-4.5.2-android-sdk/sdk/native/jni" CACHE PATH "")
    endif()
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(OpenCV_DIR "/usr/local/opencv-4.2/lib/cmake/opencv4" CACHE PATH "")
    elseif (ANDROID)
        set(OpenCV_DIR "/Users/chris/soft/opencv-4.5.0-android-sdk/sdk/native/jni" CACHE PATH "")
    endif()
endif()
find_package(OpenCV REQUIRED)







add_executable(testbed
    main.cpp
    cpu.h
    cpu.cpp
    option.h
    option.cpp
)
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


target_link_libraries(testbed PUBLIC ${OpenCV_LIBS})