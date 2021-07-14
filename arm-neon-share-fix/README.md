# ARM NEON 分享 - 例程 segfault 的修正

## 0x0 Intro
初学 arm neon 的时候， 会找一些 tutorial 看，例如下面这个：

https://www.yuque.com/docs/share/3eff70c4-c70f-40df-b0af-df9fa7365d3c?#

然而这个 tutorial 里的例子， 在 android armv8 的 Release 模式下运行， 会发生 segment fault。（真是让新手沮丧）

segment fault 的原因是它代码中的 `vector_dot_assembly()` 写的不对：对于会修改的参数，应当放到 write list 而不是 read list。

## 0x1 GCC arm 汇编语法

简单回顾一下 arm 汇编中输入参数、clobber 的格式：

```c++
asm volatile(
    "code"
    : output operand list
    : input operand list
    : clobber list
);
```

而 input/output 还可以有具体的修饰符， `r` 表示只读， `+r` 表示读+写。


## 0x2 代码修改

原来的（错误的）写法：
```c++
void vector_dot_assembly(float32_t const* src1, float32_t const * src2, float32_t *dst, uint32_t count)
{
  asm volatile (
  "fmul v3.4s, v5.4s, v5.4s \n"
  "1: \n"
  "ld1 {v0.4s}, [%[src1]], #16 \n"
  "ld1 {v1.4s}, [%[src2]], #16 \n"
  "fmul v2.4s, v0.4s, v1.4s \n"
  "fadd v3.4s, v3.4s, v2.4s \n"
  "subs %[count], %[count], #4 \n"
  "bgt 1b \n"
  "st1 {v3.4s}, [%[dst]] \n"
  : [dst] "+r" (dst)
  : [src1] "r" (src1), [src2] "r" (src2), [count] "r" (count)
  : "memory", "v0", "v1"
  );
}
```

修正后的写法：
```c++
void vector_dot_assembly(float32_t const* src1, float32_t const * src2, float32_t *dst, uint32_t count)
{
    asm volatile (
    "fmul v3.4s, v5.4s, v5.4s \n"
    "1: \n"
    "ld1 {v0.4s}, [%[src1]], #16 \n"
    "ld1 {v1.4s}, [%[src2]], #16 \n"
    "fmul v2.4s, v0.4s, v1.4s \n"
    "fadd v3.4s, v3.4s, v2.4s \n"
    "subs %w[count], %w[count], #4 \n"
    "bgt 1b \n"
    "st1 {v3.4s}, [%[dst]] \n"
    : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2), [count] "+r" (count)
    : 
    : "memory", "v0", "v1", "v2", "v3", "v5"
    );
}
```


## 0x4 结果
XiaoMI 11 上的运行结果
```
(base) zz@home% ./android-arm64-run.sh  
android-arm64/testbed: 1 file pushed, 0 skipped. 63.3 MB/s (27720 bytes in 0.000s)
c spend time :3146
intrinsics spend time :716
assembly spend time :751

intrinsics result compare with c result diff:0.001953
assembly result compare with c result diff:0.000977
```

## 0x5 工程获取

