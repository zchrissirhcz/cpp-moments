# message("begin child cmake")
# set cross-compiled system type, it's better not use the type which cmake cannot recognized.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# when gcc-arm-aarch64-none-linux-gnu was installed, toolchain was available as below:
set(CMAKE_C_COMPILER "aarch64-none-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-none-linux-gnu-g++")

# set searching rules for cross-compiler
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# other needed options
#set(GNUEABIHF_C_FLAGS "-Wall -fPIC -mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -mthumb-interwork -marm" CACHE INTERNAL docstring)
#set(GNUEABIHF_C_FLAGS "-Wall -fPIC -Wl,-rpath-link=/home/zz/soft/ffmpeg/aarch64-none-linux-gnu/lib" CACHE INTERNAL docstring)
set(GNUEABIHF_C_FLAGS "-Wall -fPIC" CACHE INTERNAL docstring)
set(GNUEABIHF_CXX_FLAGS "")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GNUEABIHF_C_FLAGS}" CACHE STRING "C flags" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNUEABIHF_C_FLAGS} ${NDK_CXX_FLAGS}" CACHE STRING "C++ flags" FORCE)


message(STATUS "=== in toolchain, CMAKE_C_COMPILER is ${CMAKE_C_COMPILER}")

# message("end child cmake")

