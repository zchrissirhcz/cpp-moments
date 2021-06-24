## gdb 资料

官方资料
- 官网 https://sourceware.org/gdb
- 文档： https://sourceware.org/gdb/current/onlinedocs/gdb.pdf
- wiki： https://sourceware.org/gdb/wiki/

视频教程
- https://www.bilibili.com/video/BV1EK411g7Li  清晰易懂的入门tutorial

调试原理举例
- https://zhuanlan.zhihu.com/p/336922639



## 配置 gdb

gdb 配置文件是 `~/.gdbinit` ，gdb 带 Python 支持情况下（例如 ubuntu 20.04 apt 装的 gdb 9.2），配置文件中可以使用 Python 进行配置。

我用的是 [dashboard](https://github.com/cyrus-and/gdb-dashboard/)，拷贝到个人 `~/.gdbinit` , 并稍作布局的修改:

```
dashboard -layout stack assembly source !variables
```

![gdb-dashboard截图](gdb-dashboard-snapshot.png)

## gdb 调试， level 1

### 编译阶段：开启符号信息

打开 `-g` 参数：

```bash
g++ main.cpp -g
```

如果是 CMake 的构建，可以简单设定为：
```cmake
set(CMAKE_BUILD_TYPE Debug)
```

注意到 `CMAKE_BUILD_TYPE=Debug` 是组合了 `-g` 和 不带优化开关等编译选项，也可以只开符号信息：
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
```

### 调试开始：加载文件

- 直接加载二进制：
```bash
gdb a.out             # 加载二进制文件，一切都没有执行处于最开始
```

- 加载二进制和 core 文件：
```bash
ulimit -c unlimited   # 允许生成 core 文件
./a.out               # 运行一个能触发 segfault 的二进制，会生成 core 文件
gdb ./a.out core      # 加载二进制和 core 文件，处于 segfault 事发现场
```

- 附加到进程
```bash
gdb program 1234
gdb -p 1234
```


### 调试命令

**普通断点**

- `b 函数名`
- `b 行号`

**watchpoint** 

当表达式的值变化时，就停下来，也叫做 data breakpoint

- `watch 表达式`

**运行**

`r` 或 `run`

**执行下一行（函数级）**

`s` 或 `step`

**执行下一个指令（汇编级）**

`si`

**打印变量**

常规打印： `p 变量名`

16进制显示： `p/x 变量名`

**退出gdb**

`q` 或 `quit`

**执行shell命令**

- `shell pwd`
- `shell cat ~/.pathrc`


## gdb 调试， level 2

### argv 传参

```bash
gdb --args ./a.out  arg1  arg2
```

```
$ gdb executablefile
(gdb) r arg1 arg2 arg3
```

```
(gdb) set args arg1 arg2
(gdb) show args
```

### 多个源文件时

**指定断点**
```gdb
b file1.cpp: linenum
```

**路径切换**

不同文件夹中的文件， 可以用 directory 命令修改路径

```gdb
(gdb) directory ../test1/
Source directories searched: /mnt/hgfs/share/C++/GDB/ManyFiles/../test1:$cdir:$cwd 
(gdb) b test1.cpp:2
Breakpoint 1 at 0x4008b2: file ../test1/test1.cpp, line 2.
(gdb) r
Starting program: /mnt/hgfs/share/C++/GDB/ManyFiles/test 

Breakpoint 1, test1 () at ../test1/test1.cpp:5
5    cout<<"test1"<<endl;
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x00000000004008b2 in test1() at ../test1/test1.cpp:2
breakpoint already hit 1 time
(gdb) 
```


### TODO

1. 切换 frame / call stack ?
2. 多文件时，编辑器 、 IDE 集成支持？
3. 修改变量？


