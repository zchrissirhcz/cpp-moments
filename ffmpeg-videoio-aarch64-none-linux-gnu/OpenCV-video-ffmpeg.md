# OpenCV 交叉编译：基于 FFMPEG 的 VideoIO 模块

## 0x0 编译的原因

想在目标平台 `aarch64-none-linux-gnu-g++` 上读取视频，作为算法 SDK 的输入数据；交叉编译了带 `videoio` 模块的 `OpenCV`，遇到 `VideoCapture` 无法读取 `.mp4` 格式视频的问题。排查后发现：

1. 尽管不指定 FFMPEG，VideoIO 模块也能编译出来，但对应的编解码器 FOURCC 只能编解码 `.avi` 格式的视频，无法满足实际需求。

2. `videoio` 模块可以有不同 backend，在 PC 上先前主要是 Windows 平台，使用了预编译的 ffmpeg dll，没有对编解码 backend 引起重视；而 `aarch64-none-linux-gnu` 这样的嵌入式平台，没找到现成的 `FFMPEG` 库（如果有，请留言告知），于是手动编译 `FFMPEG`，并在 OpenCV 的 cmake 构建中集成。

## 0x1 环境

- Host: Ubuntu 20.04
- 交叉编译工具: aarch64-none-linux-gnu-g++
- cmake 3.20

## 0x2 ffmpeg

**下载**
```bash
git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
cd ffmpeg
```

**切分支**

直接用 master HEAD commit 的 ffmpeg，由于 API 兼容性被破坏，导致 OpenCV 编译失败，需要切回早一点的 ffmpeg 版本：
```bash
git checkout -b master_20210303 573f05a753
```

**编译安装**

执行配置和编译:
```bash
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
```

注：configure 后提示说没找到 aarch64-none-linux-gnu-pkg-config，尝试交叉编译 pkg-config 发现很繁琐，GLIB 也要重新编译，就放弃了；好在这个交叉编译的 pkg-config 缺失，并没导致 ffmpeg 和 opencv 编译失败。


**cmake支持**

ffmpeg 官方不提供 cmake 构建支持，github 上找到的 ffmpeg cmake 写得很长但不起作用；简单起见，用如下 Python 代码生成 `ffmpeg-config.cmake`（注意库版本为手动硬编码，库的顺序也很重要）

`gen-ffmpeg-cmake-config.py`：

```python
#!/usr/bin/env python
#coding: utf-8

"""
Generate ffmpeg-config.cmake
"""

# note: the order matters
# ref: https://blog.csdn.net/zijin0802034/article/details/52779791
ffmpeg_lib_desc = {
    'libavformat': '58.71.100',
    'libavdevice': '58.12.100',
    'libavcodec': '58.129.100',
    'libavutil': '56.67.100',
    'libswscale': '5.8.100',
    'libswresample': '3.8.100',
    'libavfilter': '7.109.100',
    'libavresample': '4.0.0',
}

content_lines = [
    'set(ffmpeg_path "${CMAKE_CURRENT_LIST_DIR}")',
    '',
    'set(FFMPEG_EXEC_DIR "${ffmpeg_path}/bin")',
    'set(FFMPEG_LIBDIR "${ffmpeg_path}/lib")',
    'set(FFMPEG_INCLUDE_DIRS "${ffmpeg_path}/include")',
    '',
]


# setup library names
content_lines.append('# library names')
content_lines.append(
    'set(FFMPEG_LIBRARIES',
)

for key, value in ffmpeg_lib_desc.items():
    lib_name = key
    lib_version = value
    content_lines.append(
        '    ${FFMPEG_LIBDIR}/' + lib_name + '.a'
    )
content_lines.append('    pthread')
content_lines.append(')')
content_lines.append('')


# setup library found
content_lines.append('# found status')
for key, value in ffmpeg_lib_desc.items():
    lib_name = key
    content_lines.append(
        'set(FFMPEG_{:s}_FOUND TRUE)'.format(lib_name)
    )
content_lines.append('')

# setup library versions
content_lines.append('# library versions')
for key, value in ffmpeg_lib_desc.items():
    lib_name = key
    lib_version = value
    content_lines.append(
        'set(FFMPEG_{:s}_VERSION {:s})'.format(lib_name, lib_version)
    )
content_lines.append('')

# final stuffs
content_lines.append('set(FFMPEG_FOUND TRUE)')
content_lines.append('set(FFMPEG_LIBS ${FFMPEG_LIBRARIES})')

# summary up and write
fout = open('ffmpeg-config.cmake', 'w')
for line in content_lines:
    fout.write(line + '\n')
fout.close()
```

其中版本号的设定，是为了在 OpenCV 中找到 FFMPEG 后，能通过最低版本检查。

将这个文件，放在 ffmpeg 安装路径的根目录，并运行，得到 `ffmpeg-config.cmake`，则后续可用 `find_package(FFMPEG)` 使用；

## 0x3 opencv

### 1. 去掉 gapi 模块

使用的是 OpenCV 4 的最新版，对应到 4.5.2 tag 之后。

我的环境下，会硬编码 GAPI 的静态库，导致换了机器后，交叉编译的 OpenCV 无法被正确链接。因而删掉了 gapi 目录。


### 2. toolchain

交叉编译阶段用到的 toolchain 文件里，如果后续用的是 ffmpeg 动态库，则需要在 toolchain 里指定 rpath。本人最终用的静态库，因而未设 rpath。

如下脚本，保存为 `build/aarch64-none-linux-gnu.toolchain.cmake`
```cmake
# message("begin child cmake")
# set cross-compiled system type, it's better not use the type which cmake cannot recognized.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# when gcc-arm-aarch64-none-linux-gnu was installed, toolchain was available as below:
set(CMAKE_C_COMPILER "aarch64-none-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-none-linux-gnu-g++")

# set searching rules for cross-compiler
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# other needed options
#set(GNUEABIHF_C_FLAGS "-Wall -fPIC -mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=hard -mthumb-interwork -marm" CACHE INTERNAL docstring)
#set(GNUEABIHF_C_FLAGS "-Wall -fPIC -Wl,-rpath-link=/home/zz/soft/ffmpeg/aarch64-none-linux-gnu/lib" CACHE INTERNAL docstring) # for shared libs, rpath required
set(GNUEABIHF_C_FLAGS "-Wall -fPIC" CACHE INTERNAL docstring)
set(GNUEABIHF_CXX_FLAGS "")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GNUEABIHF_C_FLAGS}" CACHE STRING "C flags" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNUEABIHF_C_FLAGS} ${NDK_CXX_FLAGS}" CACHE STRING "C++ flags" FORCE)
```

https://stackoverflow.com/questions/12427928/configure-and-build-opencv-to-custom-ffmpeg-install

### 3. 编译

用如下脚本来编译，保存为 `build/aarch64-linux-gnu.sh` 并执行

```bash
#!/bin/bash

BUILD_DIR=aarch64-none-linux-gnu
mkdir -p $BUILD_DIR
cd $BUILD_DIR

TOOLCHAIN=../aarch64-none-linux-gnu.toolchain.cmake

# to use ffmpeg shared libs, comment out the following lines
#FFMPEG_INSTALL_PATH=/home/zz/soft/ffmpeg/aarch64-none-linux-gnu
#export LD_LIBRARY_PATH=$FFMPEG_INSTALL_PATH:LD_LIBRARY_PATH
#export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$FFMPEG_INSTALL_PATH/lib/pkgconfig
#export PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR:$FFMPEG_INSTALL_PATH/lib


cmake ../.. \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_LIST=core,imgcodecs,imgproc,videoio \
    -D CMAKE_INSTALL_PREFIX=/home/zz/lib/opencv/aarch64-none-linux-gnu/4.5.2+ \
    -D BUILD_TESTS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D WITH_CUDA=OFF \
    -D WITH_VTK=OFF \
    -D WITH_MATLAB=OFF \
    -D BUILD_DOCS=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_python2=OFF \
    -D WITH_IPP=OFF \
    -D BUILD_ANDROID_PROJECTS=OFF \
    -D BUILD_ANDROID_EXAMPLES=OFF \
    -D BUILD_ANDROID_SERVICE=OFF \
    -D BUILD_SHARED_LIBS=OFF \
    -D OPENCV_FORCE_3RDPARTY_BUILD=ON \
    -D BUILD_opencv_apps=OFF \
    -D CMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -D WITH_CUDA=OFF \
    -D WITH_OPENCL=OFF \
    -D WITH_VTK=OFF \
    -D WITH_MATLAB=OFF \
    -D BUILD_DOCS=OFF \
    -D BUILD_opencv_python3=OFF \
    -D BUILD_opencv_python2=OFF \
    -D BUILD_JAVA=OFF \
    -D BUILD_FAT_JAVA_LIB=OFF \
    -D WITH_PROTOBUF=OFF \
    -D WITH_QUIRC=OFF \
    -D WITH_FFMPEG=ON \
    -D CPU_BASELINE=NEON \
    -D WITH_1394=OFF \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_FFMPEG_USE_FIND_PACKAGE=ON \
    -D FFMPEG_DIR=/home/zz/soft/ffmpeg/aarch64-none-linux-gnu

cmake --build .
cmake --install .

cd ..
```


由于 OpenCV 的 cmake 脚本中，对于手动指定自行编译的 ffmmpeg 情况下（包括 cmake 方式和 pkg-config方式），会硬编码 ffmpeg 库路径，导致把 OpenCV 安装目录打包到别的机器上会无法找到 ffmpeg 库文件（也可能是我的环境没正确配置导致），这里手动处理掉路径硬编码的问题：

**拷贝ffmpeg到OpenCV安装目录**

把编译安装的 ffmpeg 目录，拷贝一份到编译安装的 OpenCV 目录，我放在 `<opencv安装根目录>/lib/opencv4/3rdparty/ffmpeg` 目录。这是为了在新的机器上，能手动 find_package 找到 ffmpeg。

**改OpenCVModules.cmake**

提及到的 OpenCV 安装脚本，把 ffmpeg 库路径硬编码，是在 `lib/cmake/opencv4/OpenCVModules.cmake` 文件中。找到 `set_target_properties(opencv_videoio PROPERTIES` 这句，去掉里面的 `\$<LINK_ONLY:ocv.3rdparty.ffmpeg>;`。

## 0x4 使用

如前所述，用这版 OpenCV 的 videoio 模块时，要额外链接 ffmpeg。关键 cmake 写法：

```cmake
# 设置 ffmpeg 的 cmake 查找路径
set(FFMPEG_DIR "E:/lib/opencv/aarch64-none-linux-gnu/4.5.2+/lib/opencv4/3rdparty/ffmpeg/aarch64-none-linux-gnu" CACHE PATH "Directory that contains ffmpeg-config.cmake")
find_package(FFMPEG REQUIRED)

# 设置 OpenCV 的 cmake 查找路径
set(OpenCV_DIR "E:/lib/opencv/aarch64-none-linux-gnu/4.5.2+/lib/cmake/opencv4" CACHE PATH "Directory that contains OpenCVConfig.cmake")
find_package(OpenCV REQUIRED)

add_executable(testbed
   testbed.cpp
)

target_link_libraries(testbed ${OpenCV_LIBS} ${FFMPEG_LIBS})
```

## 0x5 References

https://stackoverflow.com/questions/12427928/configure-and-build-opencv-to-custom-ffmpeg-install

https://github.com/opencv/opencv/issues/19906

https://zhuanlan.zhihu.com/p/147315845

https://ffmpeg.org/download.html
