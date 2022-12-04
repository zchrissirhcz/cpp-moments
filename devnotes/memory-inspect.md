# 内存

## 概要
C/C++ 工程中经常遇到内存问题，表现为：
- 内存泄漏
- 内存非法访问
    - segmentation fault
    - 结果不符合预期
    - 偶现 crash

## 解决方案: 源码调试
- 断点
- 加 log

难点是，需要准备能复现问题的输入， 例如多个库集成时， 可能总是获取不正确真实输入。

## 解决方案: 使用 Address Sanitizer

见 [sanitizer](sanitizer)


## 解决方案: 使用 overlook.cmake
用来解决 Asan 无法发现的一些问题， 如： 函数应当返回但缺失返回值导致读写 NULL 指针， 函数没声明就使用导致64位地址被截断从而引发 crash， ...

这些问题可以简单归纳为： C/C++ 编译器不够严谨，仅仅作为 warning 报告， 而我们又往往不能把所有 warning 作为 error 处理（项目时间点等因素导致）。

overlook.cmake 则是从实际项目中遇到的 crash 等内存问题， 整理出 37 个重点 warning 作为 error 检查， 在编译阶段就捕获不规范代码， 源头上消除潜在的内存问题：
(支持 Visual Studio， GCC, Clang， Android NDK， tda4 等绝大多数编译器)

下载 [overlook.cmake](https://github.com/zchrissirhcz/overlook/blob/main/overlook.cmake) 并放在工程根目录， 然后在 CMakeLists.txt 里引入：
```cmake
...
project(xxx)
include(overlook.cmake) # 加在 project() 之后的第一行
...
```

## 解决方案：使用 Valgrind
用来检查内存泄漏、 非法内存访问（越界）。

Windows MSVC 无法使用。

Linux x64 可 apt 安装， 其他平台如 android、 tda4 等需要自行编译。

## 解决方案： 使用 VLD
用来检查内存泄漏， 仅支持 VS 平台。

下载 vld 插件： https://github.com/oneiric/vld （此版本支持 vs2019）

配置头文件和库文件， 并在代码中使用.

Debug 模式：
```c++
#include <vld.h>
```

Release 模式：
```c++
#define VLD_FORCE_ENABLE
#include <vld.h>
```

一些注意：
- 代码中尽量不要有中文， 中文可能导致 vld 无法检测到泄漏
