# 使用 gdb/lldb 调试虚函数导致的段错误

## 0x1 起因

在尝试理解 C++ 中的 std::move / RVO 等内容时，恰好在群里看到 [爬树贼溜的小松鼠](https://www.zhihu.com/people/lan-tian-89) 的文章：[Copy/move elision: C++ 17 vs C++ 11](https://zhuanlan.zhihu.com/p/379566824) 这篇文章；由于对汇编级调试不甚了解，点开了作者另一篇科普文 [如何阅读简单的汇编（持续更新)](https://zhuanlan.zhihu.com/p/368962727) 展开学习。作者给出的实际调试例子中，子类虚函数返回了空指针，导致后续访问空指针触发
segfault ，一方面展示了 gdb 的基础用法，另一方面也把 C++ 内存模型中的虚函数表串了起来。虽然作者没给出完整例子，但稍加尝试，不难构造类似的案例。

本文给出这样的具体代码，并结合本地（lldb，CLion）和线上（godbolt）的结果进行对比分析，验证正确性。在掌握了 gdb/lldb 的汇编级调试 C++ 程序的基本技能后，当面对没有代码、只有 coredump 文件（但需要符号信息）的情况下，也应该能做出类似分析。


## 0x2 代码

完整的代码如下，用来在本地复现：
```c++
#include <stdio.h>
#include <iostream>

class Base
{
public:
    virtual void f1() {}
    virtual void f2() {}
    virtual void f3() {}
    virtual void f4() {}
    virtual void f5() {}
    virtual int* hello() {
        len = 10;
        printf("Base::hello()\n");
        int* data = new int[len];
        return data;
    }
    virtual ~Base() {}
private:
    int len;
};

class Derived: public Base
{
public:
    int* hello() {
        printf("Derived::hello()\n");
        return NULL;
    }
    ~Derived() {}
};

void play(int a, Base* ptr, int& c)
{
    c = *(ptr->hello());
}

int main() {

    int a = 1;
    Base* ptr = new Derived;
    int c;
    play(a, ptr, c);

    return 0;
}
```

也可以直接看 godbolt 上的版本，有对应的汇编：
- x86-64 clang 10.0.0 https://godbolt.org/z/5rxqdd7hM


## 0x3 关闭 ASLR

本地调试环境是 x86-64 ubuntu 20.04 clang 10.0.0 (apt 安装)，后端实际使用的是 GCC 9。

gdb/lldb 在调试时默认关闭了 ASLR， Visual Studio 则默认没有关闭，需要手动设定

https://www.jianshu.com/p/d3f094b4443d


## 0x4 调试记录

### 本地调试

本地调试时，使用 CMake 来构建工程，搭配 CLion 最新版，方便在源码、反汇编、LLDB 窗口之间切换。

手动在43行设定断点，然后用进入 lldb 窗口，输入一次 `s` 命令（表示源码级的 step into）和4次 `si` 命令（表示汇编指令级的 step into）；然后输入 `dis` 命令（表示 disassembly 反汇编）。

clang 10.0.0 调试：
```
(lldb) n  // (1)此时直接触发 segfault
(lldb) dis // (2)反汇编查看
testbed`play:
    0x4011e0 <+0>:  pushq  %rbp
    0x4011e1 <+1>:  movq   %rsp, %rbp
    0x4011e4 <+4>:  subq   $0x20, %rsp
    0x4011e8 <+8>:  movl   %edi, -0x4(%rbp)
    0x4011eb <+11>: movq   %rsi, -0x10(%rbp)
    0x4011ef <+15>: movq   %rdx, -0x18(%rbp)
    0x4011f3 <+19>: movq   -0x10(%rbp), %rdi
    0x4011f7 <+23>: movq   (%rdi), %rax
    0x4011fa <+26>: callq  *0x28(%rax)   // (4)之所以不对，是因为这行call的函数返回（放rax）的不对
->  0x4011fd <+29>: movl   (%rax), %ecx  // (3)好家伙， $rax 等于 0，访问 0 地址肯定不对
    0x4011ff <+31>: movq   -0x18(%rbp), %rax
    0x401203 <+35>: movl   %ecx, (%rax)
    0x401205 <+37>: addq   $0x20, %rsp
    0x401209 <+41>: popq   %rbp
    0x40120a <+42>: retq   
(lldb) p/x $rbp
(unsigned long) $0 = 0x00007fff25d4a5a0
(lldb) p/x $rbp - 0x10
(unsigned long) $1 = 0x00007fff25d4a590
(lldb) x/gx 0x00007fff25d4a590
0x7fff25d4a590: 0x0000000001f41eb0
(lldb) x/gx 0x0000000001f41eb0
0x01f41eb0: 0x0000000000402018
(lldb) p/x 0x0000000000402018 + 0x28
(int) $2 = 0x00402040
(lldb) x/gx 0x00402040
0x00402040: 0x0000000000401310
(lldb) image lookup --address  0x0000000000401310
      Address: testbed[0x0000000000401310] (testbed.PT_LOAD[1]..text + 624)
      Summary: testbed`Derived::hello() at main.cpp:26

```


解释：

从后往前分析：

出现 crash 的是 `movl (%rax), %r8d` 这一行，由于这个 bug 是我们可以写的，显然 `rax` （存储了返回值）的值为0，因此 `$(%rax)` 这一访问空指针的操作触发了 segfault。`rax` 是由前一次指令 `callq *0x28(%rcx)` 确定的（这里和网页版略有区别，网页版的 40 是 10 进制的，0x28 是 16 进制的，相等），而具体调用的函数是在 `*0x28(%rcx)` 表示的地址确定的。`rcx`是从 `movq (%rax), %rcx` 确定的，而 `rax` 则是从 `rbp` 确定的: `movq -0x10(%rbp), %rax`。

因此我们从前往后推演，顺带打印：

1. 确定 rax 的值：
```
(lldb) p/x $rbp
(unsigned long) $0 = 0x00007fffef8ba6f0                              
(lldb) p/x $rbp - 0x10
(unsigned long) $1 = 0x00007fffef8ba6e0
(lldb) x/gx 0x00007fffef8ba6e0
0x7fffef8ba6e0: 0x0000000001773eb0
```
其中 `p/x` 是用16进制形式打印，`x/gx` 则简单理解为 dereference。`-0x10(%rbp)` 相当于是 `(%rbp - 0x10)`，也就是 `rbp` 的值减 0x10 后，再做 dereference。

2. 确定 rcx 的值：
```
(lldb) x/gx 0x0000000001773eb0                                       
0x01773eb0: 0x0000000000402018
```

3. 确定被调用函数的地址
```
(lldb) p/x 0x0000000000402018 + 0x28                                 
(int) $2 = 0x00402040
(lldb) x/gx 0x00402040
0x00402040: 0x0000000000401320
```
关于这里为什么是 `0x28` 的解释：对应到十进制 40，是由于 Base 类有 5 个虚函数 `f1~f5`，也就是说虚函数 `hello` 的地址，在 vtable 中排列时，前面还有5个兄弟姐妹。

4. 根据函数地址，从符号表中查出对应的函数名字：
```
(lldb) image lookup -a 0x0000000000401320
      Address: testbed[0x0000000000401320] (testbed.PT_LOAD[1]..         text + 640)
      Summary: testbed`Derived::hello() at main.cpp:26
(lldb) image lookup -a 0x0000000000402018
      Address: testbed[0x0000000000402018] (testbed.PT_LOAD[2]..         rodata + 24)
      Summary: testbed`vtable for Derived + 16
```


