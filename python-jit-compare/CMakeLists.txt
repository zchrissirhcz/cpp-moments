cmake_minimum_required(VERSION 3.20)
project(x)
set(CMAKE_CXX_STANDARD 11)

add_executable(testbed
    main.cpp
)

if(ANDROID)
    set(OpenCV_DIR "/home/zz/artifacts/opencv/4.5.5/OpenCV-android-sdk/sdk/native/jni")
endif()
find_package(OpenCV REQUIRED)

target_link_libraries(testbed
    ${OpenCV_LIBS}
)