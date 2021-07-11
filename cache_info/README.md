# 获得 android 手机的 cache 信息

## 0x1 目的
在移动端推理框架 [ncnn](https://github.com/tencent/ncnn) 的 arm neon 汇编代码中， 经常看到 `pld #256` 这样的预取指令， 意思是从内存搬 256bit 到 cache 。为啥是 256？ 大概是说， 是 L1 cacheline 的整数倍。 

怎样获得手上 android 手机的 L1 cache 大小， L1 cacheline 大小？

StackOverFlow 上已经有这样的问答了： [Programmatically get the cache line size on Android](https://stackoverflow.com/questions/49619909/programmatically-get-the-cache-line-size-on-android) ， 方法是调用 [cpuinfo](https://github.com/pytorch/cpuinfo) 库来打印 cache 信息; 此外在编译 cpuinfo android 库时生成的 `cpu-info`, `cache-info`, `isa-info` 这三个可执行文件，也可以获取相关信息。




## 0x2 编译 cpuinfo android 库

```bash
git clone https://github.com/pytorch/cpuinfo
cd cpuinfo
mkdir build
cd build
```

创建 `android-arm64-build.sh`:
```bash
#!/bin/bash

ANDROID_NDK=~/soft/android-ndk-r21b
TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake

BUILD_DIR=android-arm64
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -DANDROID_LD=lld \
    -DANDROID_ABI="arm64-v8a" \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCPUINFO_BUILD_UNIT_TESTS=OFF \
    -DCPUINFO_BUILD_MOCK_TESTS=OFF \
    -DCPUINFO_BUILD_BENCHMARKS=OFF \
    -DCMAKE_INSTALL_PREFIX=./install/arm64 \
    ../..

#ninja
#cmake --build . --verbose
cmake --build .

cd ..
```

## 0x3 编译获取 cache line 的程序
```c++
#include <string>
#include <sstream>
#include <cpuinfo.h>
#include <iostream>

void get_cache_info(const char* name, const struct cpuinfo_cache* cache, std::ostringstream& oss)
{
    oss << "CPU Cache: " << name << std::endl;
    oss << " > size            : " << cache->size << std::endl;
    oss << " > associativity   : " << cache->associativity << std::endl;
    oss << " > sets            : " << cache->sets << std::endl;
    oss << " > partitions      : " << cache->partitions << std::endl;
    oss << " > line_size       : " << cache->line_size << std::endl;
    oss << " > flags           : " << cache->flags << std::endl;
    oss << " > processor_start : " << cache->processor_start << std::endl;
    oss << " > processor_count : " << cache->processor_count << std::endl;
    oss << std::endl;
}

const std::string get_cpu_info()
{
    cpuinfo_initialize();
    const struct cpuinfo_processor* proc = cpuinfo_get_current_processor();

    std::ostringstream oss;

    if (proc->cache.l1d)
        get_cache_info("L1 Data", proc->cache.l1d, oss);

    if (proc->cache.l1i)
        get_cache_info("L1 Instruction", proc->cache.l1i, oss);

    if (proc->cache.l2)
        get_cache_info("L2", proc->cache.l2, oss);

    if (proc->cache.l3)
        get_cache_info("L3", proc->cache.l3, oss);

    if (proc->cache.l4)
        get_cache_info("L4", proc->cache.l4, oss);

    return oss.str();
}

int main()
{
    printf("hello\n");
    std::string info = get_cpu_info();
    std::cout << info << std::endl;

    return 0;
}
```

对应的 CMakeLists.txt:
```cmake
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
```

编译脚本和前面的类似不写了；执行脚本：
```bash
BUILD_DIR=android-arm64
DST_DIR=/data/local/tmp
EXE_FILE=testbed

adb shell "mkdir -p $DST_DIR"
adb push $BUILD_DIR/$EXE_FILE $DST_DIR
adb shell "cd $DST_DIR; chmod +x $DST_DIR/$EXE_FILE; ./$EXE_FILE"
```


## 0x4 获取信息

### 获取 cache info

```bash
cd bin
adb push cpu-info /data/local/tmp
adb push cache-info /data/local/tmp
adb shell
cd /data/local/tmp
```

### cpu-info
```
venus:/data/local/tmp $ ./cpu-info 
SoC name: Qualcomm Snapdragon 8350
Microarchitectures:
        1x Cortex-X1
        3x Cortex-A78
        4x Cortex-A55
Cores:
        0: 1 processor (0), ARM Cortex-X1
        1: 1 processor (1), ARM Cortex-A78
        2: 1 processor (2), ARM Cortex-A78
        3: 1 processor (3), ARM Cortex-A78
        4: 1 processor (4), ARM Cortex-A55
        5: 1 processor (5), ARM Cortex-A55
        6: 1 processor (6), ARM Cortex-A55
        7: 1 processor (7), ARM Cortex-A55
Logical processors (System ID):
        0 (7)
        1 (4)
        2 (5)
        3 (6)
        4 (0)
        5 (1)
        6 (2)
        7 (3)
```

### cache-info
```
venus:/data/local/tmp $ ./cache-info 
Max cache size (upper bound): 4194304 bytes
L1 instruction cache: 8 x 32 KB, 4-way set associative (128 sets), 64 byte lines, shared by 1 processors
L1 data cache: 8 x 32 KB, 4-way set associative (128 sets), 64 byte lines, shared by 1 processors
L2 data cache: 6 x 256 KB (exclusive), 8-way set associative (512 sets), 64 byte lines, shared by 1 processors
L3 data cache: 1 MB (exclusive), 16-way set associative (1024 sets), 64 byte lines, shared by 4 processors
```

### isa-info

```
venus:/data/local/tmp $ ./isa-info 
Instruction sets:
        ARM v8.1 atomics: yes
        ARM v8.1 SQRDMLxH: yes
        ARM v8.2 FP16 arithmetics: yes
        ARM v8.3 dot product: yes
        ARM v8.3 JS conversion: no
        ARM v8.3 complex: no
SIMD extensions:
        ARM SVE: no
        ARM SVE 2: no
Cryptography extensions:
        AES: yes
        SHA1: yes
        SHA2: yes
        PMULL: yes
        CRC32: yes

```


### cache line info
```
CPU Cache: L1 Data
 > size            : 32768
 > associativity   : 4
 > sets            : 128
 > partitions      : 1
 > line_size       : 64
 > flags           : 0
 > processor_start : 5
 > processor_count : 1

CPU Cache: L1 Instruction
 > size            : 32768
 > associativity   : 4
 > sets            : 128
 > partitions      : 1
 > line_size       : 64
 > flags           : 0
 > processor_start : 5
 > processor_count : 1

CPU Cache: L2
 > size            : 131072
 > associativity   : 4
 > sets            : 512
 > partitions      : 1
 > line_size       : 64
 > flags           : 0
 > processor_start : 5
 > processor_count : 1

CPU Cache: L3
 > size            : 1048576
 > associativity   : 16
 > sets            : 1024
 > partitions      : 1
 > line_size       : 64
 > flags           : 0
 > processor_start : 4
 > processor_count : 4
```