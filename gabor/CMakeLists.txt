cmake_minimum_required(VERSION 3.20)
project(x)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

add_executable(testbed
  testbed.cpp
  #testbed2.cpp
)
target_link_libraries(testbed
  ${OpenCV_LIBS}
)
