cmake_minimum_required(VERSION 3.17)

project(testbed)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)

add_executable(testbed
    get_cpu_info.cpp
)

set(cpuinfo_INCLUDE_DIRS
    /home/zz/work/cpuinfo/build/install/arm64/include
)

set(cpuinfo_LIBRARIES
    /home/zz/work/cpuinfo/build/install/arm64/lib/libcpuinfo.a
    /home/zz/work/cpuinfo/build/install/arm64/lib/libclog.a
)

target_include_directories(testbed PUBLIC ${cpuinfo_INCLUDE_DIRS})
target_link_libraries(testbed ${cpuinfo_LIBRARIES} log)