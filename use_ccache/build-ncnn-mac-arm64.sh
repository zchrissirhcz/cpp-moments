#!/bin/bash

# put this script under ncnn/build directory.

ARTIFACTS_DIR=$HOME/artifacts
BUILD_DIR=mac-arm64

cmake \
    -S .. \
    -B $BUILD_DIR \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCMAKE_BUILD_TYPE=Release \
    -DNCNN_BUILD_EXAMPLES=ON \
    -DNCNN_PYTHON=OFF \
    -DNCNN_VULKAN=OFF \
    -DNCNN_OPENMP=OFF

time cmake --build $BUILD_DIR -j