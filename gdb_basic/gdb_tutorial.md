# 跟我一起用 gdb

## 0x0. Why gdb

gdb 是什么： Linux 下的 C/C++ 调试器。

能否不用 gdb？ 可以用 CLion （几乎不用配置，会点 makefile/cmake 就行了）， 也可以用 VSCode （配置 launch.json/task.json ）。但是：汇编级别的调试， VSCode 目前还做不了， CLion 虽然内置了 LLDB ，但是和源码的联动还是比较差， 而且 CLion 收费、有时候内存消耗太多。

能否不直接用 gdb ？ 搜索过一些解决方法，但是基本都不靠谱：
- ddd ： 界面太丑，我放弃了
- kdbg ： 编译安装时的依赖卡了我一次，装好后无法加载可执行文件， 放弃
- KDevelop ： 作为 IDE 勉强能用 ， 但断点调试始终跑不起来 ， 放弃
- qtcreator ： 对于大工程来说，基本上比 KDevelop 好用， 创建的 C++ 工程也能用 CMake 的模板， 调试阶段也有 valgrind 等各种工具集成可用； 但是，汇编级别的调试， 还是没法进行：看不到汇编。
- clion： 在汇编调试方面，比 QtCreator 更近一步； 但是还是不完美， 而且要收费。

用 gdb 的好处：
- 轻量级：比 CLion 轻量，表现在内存占用上
- 通用性好：即使是 ssh 连接过去，也能用
- 汇编级别调试： 完全可以
- 还有相比于 Visual Studio 更强大的一些功能： reverse debug ， 修改变量 等

用 gdb 的难点：
- 周边没有人会（或者说，特别会） gdb， 需要自己在网络上找资料、找例子、多尝试
- 网上找到的资料，初级的太多
    - 单个文件： 很多都是在讲 单个.c 文件的调试
- 命令比较多，掌握起来往往需要搭配具体的案例，才比较方便；只有一个 hello world 的话，没法展示出来
- 讲 gdb 配置的也比较少， 个人觉得配置还是挺重要的
- 可能 《CSAPP》 这本书里面都覆盖到了， 所以大家都没有在网络上发布？

总结一下：
- 必要性： 不用、不直接用 gdb 也能调试， 但是对 segfault 的调试就差了一个 level ， 效率明显不足
- 困难：   对初学者来说，易用性不好， 大多的视频介绍过于浅显不够接近实际工程

环境简单说明：
- gdb：系统自带的 gdb
- 编辑器： 用的是 VSCode

## 0x1. 配置 gdb

gdb 配置文件是 `~/.gdbinit` ，gdb 带 Python 支持情况下（例如 ubuntu 20.04 apt 装的 gdb 9.2），配置文件中可以使用 Python 进行配置。

我用的是 [dashboard](https://github.com/cyrus-and/gdb-dashboard/)，拷贝到个人 `~/.gdbinit` , 并稍作布局的修改:

```
dashboard -layout stack assembly source !variables
```

![gdb-dashboard截图](gdb-dashboard-snapshot.png)


再复杂的配置暂时不会、也不需要。

## 0x2. 通过例子学习命令

### 1. 查看源码

`frame` 命令后，再执行 `list`.

或用 `where` / `frame` 命令后， 获得行号, 然后结合 VSCode 查看。

### 2. 查看断点

`info breakpoints`

简写为 `i b`

### 3. 打印变量

`p 表达式`  一次性的打印

`display 变量`  记住某个变量， 随后 `display` 会打印所有记住的变量的值


### 4. call stack 切换

切换到上一层call stack： `up`
切换到下一层call stack： `down`
打印所有call stack: `backtrace`    每个 stack 用编号区分
选择某个 stack， 切换过去： `frame stack的编号`

这几个命令互相为alias：
- `backtrace`
- `where`
- `info stack`