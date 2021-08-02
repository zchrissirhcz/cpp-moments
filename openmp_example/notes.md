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