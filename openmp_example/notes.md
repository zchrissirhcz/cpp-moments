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

https://hpc.llnl.gov/tuts/openMP/ 一个入门tutorial

omp-hands-on-SC08.pdf （google搜索。位于 https://www.openmp.org  的一个文件）

官方文档pdf版本： openmp-TR6.pdf

export  OMP_DISPLAY_ENV="TRUE" 



