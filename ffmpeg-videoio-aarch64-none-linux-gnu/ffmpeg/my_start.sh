#!/bin/bash

#git checkout -b master_20210303 573f05a753

./configure \
    --prefix=/home/zz/soft/ffmpeg/aarch64-none-linux-gnu \
    --shlibdir=/home/zz/soft/ffmpeg/aarch64-none-linux-gnu/lib \
    --disable-shared \
    --disable-doc \
    --arch=aarch64 \
    --cross-prefix=aarch64-none-linux-gnu- \
    --target-os=linux \
    --enable-avresample

make -j8
make install
