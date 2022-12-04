被简写为 CRT. 称为“运行库”。

如果一个静态库文件，被另一个可执行目标所链接， 但由于分别使用了不同的 CRT， 会导致链接不上。

例如：
> opencv_core460.lib(matrix.obj) : error LNK2038: mismatch detected for 'RuntimeLibrary': value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease' in testbed.obj [D:\a\opencv-action\opencv-action\test\build\testbed.vcxproj]

上述错误是在编译 OpenCV 静态库（`-DBUILD_SHARED_LIBS=OFF`）时，未指定 `BUILD_WITH_STATIC_CRT` 取值（默认为 ON），导致编译出的 OpenCV 静态库使用了静态的 CRT。
而链接阶段发生在， 为可执行目标 testbed 链接 OpenCV 静态库。 可执行目标 testbed 没有改 cmake 默认的 CRT 设置， 默认用的是动态的 CRT。因而发生了冲突。
（https://github.com/cvpkg/opencv-action/actions/runs/3076268182/jobs/4970306922）

| 库名称      | 选项 | 解释   |
| ----------- | --- | ------- |
| libcmt.lib  | /MT  | 将本机 CRT 启动静态链接到你的代码。            |
| libcmtd.lib | /MTd | 静态链接本机 CRT 启动的调试版本。 不可再发行。 |
| msvcrt.lib  | /MD  | 与 DLL UCRT 和 vcruntime 一起使用的本机 CRT 启动的静态库。 |
| msvcrtd.lib | /MDd | 与 DLL UCRT 和 vcruntime 一起使用的本机 CRT 启动调试版本的静态库。 不可再发行。 | 

https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/crt-library-features?view=msvc-170

**简单的结论：在编静态库目标时候，不要认为直接编译就好了，CMake默认用的CRT是MD/MDd, 而 gtest 和 OpenCV 则在检查到 BUILD_SHARED_LIBS=OFF 时强行替换 CRT 为 MT/MTd, which is 非常蛋疼： 往往是找到了 OpenCV/gtest, 但和你自行编译的可执行程序无法正确链接。**
