cmake_minimum_required(VERSION 3.20)
project(my_utest)
#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("output_dir.cmake")

add_executable(testbed
  #utest_v1.cpp
  #utest_v2.cpp
  #utest_v3.cpp
  #utest_v4.cpp
  #utest_v5.cpp
  #utest_v6.cpp
  #utest_v6_gtest.cpp
  #utest_v7.cpp
  #utest_v8.cpp
  #utest_v9_demo.cpp
  utest_v9.cpp
  #utest_v9_demo2.cpp
  
  # gft_demo.cpp
  # gft/test.cpp
  # gft/log.cpp
)
target_include_directories(testbed PUBLIC ${CMAKE_SOURCE_DIR})

#--- GoogleTest
if(ANDROID)
  set(GTest_DIR "${ARTIFACTS_DIR}/googletest/1.11.0/android-arm64/lib/cmake/GTest" CACHE PATH "Directory that contains GTestConfig.cmake")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(GTest_DIR "${ARTIFACTS_DIR}/googletest/1.11.0/linux-x64/lib/cmake/GTest" CACHE PATH "Directory that contains GTestConfig.cmake")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(GTest_DIR "${ARTIFACTS_DIR}/googletest/1.11.0/windows-x64/lib/cmake/GTest" CACHE PATH "Directory that contains GTestConfig.cmake")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  #set(GTest_DIR "${ARTIFACTS_DIR}/googletest/1.11.0/mac-x64/lib/cmake/GTest" CACHE PATH "Directory that contains GTestConfig.cmake")
  set(GTest_DIR "$ENV{HOME}/.sled/artifacts/googletest/release-1.11.0/lib/cmake/GTest" CACHE PATH "Directory that contains GTestConfig.cmake")
else()
  message(WARNING "GTest_DIR not set yet")
endif()
message(STATUS ">>> GTest_DIR is: ${GTest_DIR}")
find_package(GTest REQUIRED)


target_link_libraries(testbed
  GTest::gtest
  GTest::gtest_main
)
