#!/bin/bash

BUILD_DIR=android-arm64
DST_DIR=/data/local/tmp
#EXE_FILE=testbed
# yuv_rgb_test  area_resize_test
EXE_FILE=testbed
#LOCAL_DIR=/home/zz/work/arccv/aidepth_pp_dbg/assets
LOCAL_DIR=/home/zz/data

adb shell "mkdir -p $DST_DIR"
adb push $BUILD_DIR/$EXE_FILE $DST_DIR
adb shell "cd $DST_DIR; chmod +x $DST_DIR/$EXE_FILE; ./$EXE_FILE"

SAVE_DIR=/home/zz/data/rgb2gray/android
adb pull /data/local/tmp/rgb2gray/ASRDebug_0_7680x4320_gray.png             $SAVE_DIR
adb pull /data/local/tmp/rgb2gray/ASRDebug_0_7680x4320_gray_omp.png         $SAVE_DIR
adb pull /data/local/tmp/rgb2gray/ASRDebug_0_7680x4320_gray_fixed.png       $SAVE_DIR
adb pull /data/local/tmp/rgb2gray/ASRDebug_0_7680x4320_gray_fixed_omp.png   $SAVE_DIR
adb pull /data/local/tmp/rgb2gray/ASRDebug_0_7680x4320_gray_fixed_simd.png  $SAVE_DIR