cmake_minimum_required(VERSION 3.20)
project(qtest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("output_dir.cmake")

add_executable(testbed
  #v11_gtest.cpp
  #v12_gtest.cpp
  #v13_gtest.cpp
  #v14_gtest.cpp
  #v15_gtest.cpp
  v16_gtest.cpp
)
target_include_directories(testbed PUBLIC ${CMAKE_SOURCE_DIR})

add_executable(qtest
  #v11_qtest.cpp
  #v12_qtest.cpp
  #v13_qtest.cpp
  #v14_qtest.cpp
  #v15_qtest.cpp
  v16_qtest.cpp
)

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

