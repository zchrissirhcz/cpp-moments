#!/bin/bash


# 把这个脚本放在 ~/work/opencv4/build/ 目录下，添加执行权限，然后执行

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=~/soft/opencv-4.5 \
    -DBUILD_TESTS=OFF \
    -DBUILD_LIST=core,imgcodecs \
    -DBUILD_PERF_TESTS=OFF \
    -DWITH_CUDA=OFF \
    -DWITH_VTK=OFF \
    -DWITH_MATLAB=OFF \
    -DBUILD_DOC=OFF \
    -DBUILD_opencv_python3=OFF \
    -DBUILD_opencv_python2=OFF \
    -DWITH_IPP=OFF \
    -DWITH_OPENCL=OFF \
    -DOPENCV_GENERATE_PKGCONFIG=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCPU_BASELINE="SSE;SSE2;SSE3;SSSE3;SSE4_1;SSE4_2;POPCNT;AVX;AVX2"

cmake --build .
cmake --install .
