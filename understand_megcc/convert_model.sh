#!/bin/bash

mkdir -p kernel_yolox_s_arm # 这个目录是 yolox_arm.json 里指定的， 但是需要手动创建
mgb-to-tinynn --json="./yolox_arm.json" --arm64v7

#python3 ~/soft/megcc/runtime/scripts/runtime_build.py --cross_build --kernel_dir ./kernel_yolox_s_arm/ --remove_old_build --cross_build_target_arch aarch64
# runtime_build.py 执行过程中会打印具体的命令， 搭配上一行的参数， 等效于执行了如下两个命令

ANDROID_NDK=~/soft/android-ndk-r21e
TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake

BUILD_DIR=kernel_yolox_s_arm/runtime
TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake
cmake -G Ninja \
    -S ../runtime \
    -B ${BUILD_DIR} \
    -D CMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -D ANDROID_ABI=arm64-v8a \
    -D ANDROID_NATIVE_API_LEVEL=21 \
    -D CMAKE_INSTALL_PREFIX=${BUILD_DIR}/install \
    -D RUNTIME_KERNEL_DIR=/home/zz/soft/megcc/yolox_example/kernel_yolox_s_arm \
    -D CMAKE_BUILD_TYPE=Release \
    -D TINYNN_SHARED_ENABLE=OFF \
    -D TINYNN_SANITY_ALLOC=OFF \
    -D TINYNN_ACHIEVE_ALL=OFF \
    -D TINYNN_DUMP_TENSOR=OFF \
    -D TINYNN_CALLBACK_ENABLE=OFF \
    -D TINYNN_PROFILE_KERNEL=OFF \
    -D TINYNN_ENABLE_ASAN=OFF

ninja -C ${BUILD_DIR} install/strip