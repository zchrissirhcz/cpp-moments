#!/bin/bash

ANDROID_NDK=~/soft/android-ndk-r21e
TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake

#mkdir -p build_arm64 && cd build_arm64 && mkdir -p install
BUILD_DIR=build_arm64

cmake \
    -S . \
    -B ${BUILD_DIR} \
    -GNinja \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install \
    -DRUNTIME_KERNEL_DIR=$PWD/kernel_yolox_s_arm \
    -DOpenCV_DIR=$PWD/OpenCV/sdk/native/jni/abi-arm64-v8a

ninja -C ${BUILD_DIR} install/strip