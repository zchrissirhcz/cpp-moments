OpenCV：需要编译debug版本然后安装。可参考 build-opencv.sh 脚本

调试方法：我用的 VSCode

Ctrl Shift P, tasks, 有预先创建好的3个task
- cmake build
- cmake clean
- make


对于这个bug，相关链接
[ WARN:0] global /home/zz/work/opencv/modules/core/src/matrix_expressions.cpp (1334) assign OpenCV/MatExpr: processing of multi-channel arrays might be changed in the future: https://github.com/opencv/opencv/issues/16739
