#!/bin/bash

ANDROID_NDK=~/soft/android-ndk-r21b
#ANDROID_NDK=/Users/chris/soft/android-ndk-r21
TOOLCHAIN=$ANDROID_NDK/build/cmake/android.toolchain.cmake

echo "ANDROID_NDK is $ANDROID_NDK"
echo "TOOLCHAIN is: $TOOLCHAIN"

BUILD_DIR=android-arm32
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -DANDROID_LD=lld \
    -DANDROID_ABI="armeabi-v7a" \
    -DANDROID_ARM_NEON=ON \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release \
    -DCPUINFO_BUILD_UNIT_TESTS=OFF \
    -DCPUINFO_BUILD_MOCK_TESTS=OFF \
    -DCPUINFO_BUILD_BENCHMARKS=OFF \
    -DCMAKE_INSTALL_PREFIX=./install/arm32 \
    ../..

#ninja
#cmake --build . --verbose
cmake --build .

cd ..