#!/bin/bash

adb push build_arm64/install/bin/yolox_test /data/local/tmp/
adb push dog.jpg /data/local/tmp/
adb push kernel_yolox_s_arm/yolox_s.tiny /data/local/tmp/
adb shell "cd /data/local/tmp; chmod +x ./yolox_test; ./yolox_test yolox_s.tiny --input=dog.jpg --output=my_out.jpg"