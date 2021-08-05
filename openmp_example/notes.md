`#pragma omp parallel for` 这条语句是用来指定后面的for循环语句变成并行执行的，当然for循环里的内容必须满足可以并行执行，即每次循环互不相干，后一次循环不依赖于前面的循环。


```
(base) zz@home% ./a.out
test, single thread: took 0.003588 ms
test, multiple thread: took 0.000214 ms
```
开启OpenMP后，相比于不开启openmp，时间开销是10倍的关系。

`#pragma omp parallel for` 自动分匹配线程数量，通常等于 CPU 数量。

`#pragma omp parallel num_threads(n)` 手动设定n个线程的并行执行。

只调用 `#pragma omp parallel for`，不链接 openmp。则根本没有加速效果！（注意，同样的代码，两次运行，第二次容易比第一次快）。

## 资源
openmp官方对支持openmp的编译器和工具的记录： https://www.openmp.org/resources/openmp-compilers-tools/

https://hpc.llnl.gov/tuts/openMP/ 一个入门tutorial， 中文翻译：https://www.jianshu.com/p/9931c05f4058

omp-hands-on-SC08.pdf （google搜索。位于 https://www.openmp.org  的一个文件）

官方文档pdf版本： openmp-TR6.pdf

export  OMP_DISPLAY_ENV="TRUE" 

## OpenMP: An Industry Initiative in Support of Protable SMP APplication Software

当摩尔定律遭遇功耗墙 Power Wall。
功耗墙、温度墙，通俗解释： http://www.pcpop.com/article/4515263.shtml

## 实践

1. 获取 openmp 版本信息 

get_openmp_version 目录

2. rgb2gray 用 openmp 优化

限定 release 模式。8K 大图。
在 linux x64 有明显优化效果： 66ms => 12ms
在 android arm64 上反而效果不好： 78ms => 101ms（浮点）,  9ms =>56 ms（定点）
原因：有大小核的区分。先绑定大核心，再设定线程数量。
绑定大核心的前提下，4线程、2线程哪个更快？ncnn的benchmark跑出来，是4线程最快；但我的rgb2gray函数的neon+定点化版本，2线程最快；浮点数版本，或单纯的定点化版本，都是4核最快。猜测：每个线程上单次运行的时间如果特别短，那么线程同步等操作反而会增加耗时。。blocktime默认200，但修改为20，或其他数值，对性能的影响，目前看不是太明显。