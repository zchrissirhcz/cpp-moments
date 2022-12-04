# 代码风格

## 著名的代码风格
有的叫 Coding Conventions, 有的叫 Coding Style, 还有的叫 Coding Standards

- Qt: [Qt Coding Conventions](https://wiki.qt.io/Coding_Conventions)
- Google C++: [Google C++ Coding Style](https://google.github.io/styleguide/cppguide.html)
- Google C++: [Google C++ Coding Style 中文版](https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/contents/)
- OpenCV: [The OpenCV Coding Style Guide](https://github.com/opencv/opencv/wiki/Coding_Style_Guide)
- LLVM: [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)
- Mozilla: [C++ Coding style](https://firefox-source-docs.mozilla.org/code-quality/coding-style/coding_style_cpp.html)
- Chromium: [Chromium C++ style guide](https://chromium.googlesource.com/chromium/src/+/HEAD/styleguide/c++/c++.md)
- Chromium: [C++ Dos and Don'ts](https://chromium.googlesource.com/chromium/src/+/HEAD/styleguide/c++/c++-dos-and-donts.md)
- GNU: [GNU Coding Standards (聚焦于 C 语言)](https://www.gnu.org/prep/standards/standards.html)
- Webkit: [Code Style Guidelines](https://webkit.org/code-style-guidelines/)
- Bloomberg: [BDE C++ Coding Standards](https://bloomberg.github.io/bde/knowledge_base/coding_standards.html)
- isocpp的建议：[Coding Standards](https://isocpp.org/wiki/faq/coding-standards)
- C++ Core Guidelines: [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- the JSF air vehicle C++ coding standards: [the JSF air vehicle C++ coding standards](http://stroustrup.com/JSF-AV-rules.pdf)
- KDE: [Policies/Library Code Policy](https://community.kde.org/Policies/Library_Code_Policy)
- KDE: [Policies/Binary Compatibility Issues With C++](https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B)

## 命名的风格
### Camel Case (驼峰法)
1. upper camel case (大驼峰)
第一个单词首字母大写，其余单词也是首字母大写
```
FirstName, UserPhone
```

2. lower camel case (小驼峰)
第一个单词首字母小写，其余单词首字母大写
```
firstName, userPhone
```

### Snake Case
单词和单词之间使用下划线分隔
```
first_name, user_phone
```

## kebab Case (spinal-case, Train-Case, Lisp-case)
单词和单词之间使用符号连接， 有多种称呼
```
first-name,  user-phone
```

## 避免使用的命名
>  Names containing double underscore or beginning with an underscore and a capital letter are reserved for implementation (compiler, standard library) and should not be used (e.g. __reserved or _Reserved).

即： 双下划线开头的标识符， 以及单个下划线开头并且紧跟大写字母的标识符， 是保留字： 只能给编译器和标准库使用， 我们不应该使用； 即使现在不冲突， 未来仍可能冲突。


## 更多参考
[https://en.wikipedia.org/wiki/Naming_convention_(programming)](https://en.wikipedia.org/wiki/Naming_convention_(programming))


## 日常代码风格
这里列出个人认为日常应该遵循的 C++ 代码风格。

### 1. 缩进
- 头文件包含， 和第一个函数之间， 需要空号
- 使用4个空格作为缩进。如果是Linux下用vim做了代码修改，需要在 git commit 后， 检查缩进是否正确，不正确的话需要考虑调整 vim 配置， 或用其他方式编辑（如vscode remote）。
- editorconfig 插件
- clang-format 插件

### 2. 变量命名
- 保持一致的大小写规则：file_Path => file_path
- 修复 typo: `get_image_imfo` => `get_image_info`
- C文件句柄：个人建议用固定命名，如
    - `FILE* fin = fopen("x.txt", "r")`
    - `FILE* fout = fopen("x.txt", "w")`
    - 如果出现多次，可使用 `fin1`, `fin2` 等
- 定义新类型：类型名的首字母大写，如 `class MyImage`


### 3. 函数命名
- 尽可能保持明确含义，如 `get_bfile_height()` 的 bfile 含义比较模糊。
- 可改为`get_binary_file_height()` ，或在函数开头增加注释。
- 注释规则我们使用 doxygen 风格的注释。
- 定义函数： ToyVision 中使用 `getImageInfo()` 风格， 平时自己写可以用 `get_image_info()` 风格。   `GetImageInfo()` 和 `Get_Image_Info()` 两种风格，相对来说不推荐用。


### 4. 注释
注释符号后应当空格，再写具体的注释

如果注释位于一行行尾，并且以中文作为结束字符，则应当添加英文状态的`.`字符。
例如:
```c++
#include <stdio.h> //二
#include <vld.h>
```
改为
```c++
#include <stdio.h> //二.
#include <vld.h>
```
原因： 在 Visual Studio 中使用 vld 的时候，中文注释导致了 vld 无法正常工作， 而中文注释结尾添加空格后， vld 恢复正常工作。

### 5. 多用const
`char* relative_path = "../../image/";` 需改为
`const char* relative_path = "../../image/";`

### 6. 不必要打印
功能函数内部，在调试完毕后，尽可能不保留打印。
需要编写对应的单元测试。

### 7. 单元测试
使用 gtest 框架， 在编写的  `TEST(xx, yy)` 中，
- 准备输入数据
- 调用功能函数， 获得实际结果 actual
- 准备输入数据的预期结果 expected
- 比较 expected 和 actual
- 尽可能达到100%覆盖率

### 8. 文件路径
- 如果要使用绝对路径， 应当考虑 windows，linux， android 三个或更多平台
- 如果测试图片、文件是固定的，建议放在工程目录下，通过 cmake 方式在构建阶段拷贝到 CMAKE_BINARY_DIR， 代码中使用 `./image.png` 方式使用。

### 9. 增强代码复用性
- 思考：当前目录下写的代码， 怎样在下一个工程里使用？
