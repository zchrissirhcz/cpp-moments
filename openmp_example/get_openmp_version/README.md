## 原理
通过代码里打印 _OPENMP 的值，根据查表，得到 openmp 版本。

注意，开启 `OMP_DISPLAY_ENV` 环境变量为 TRUE 或者 VERBOSE，得到的额外 log 输出中的 _OPENMP ，和 C/C++ 代码中得到的，不一样！。。

## 实测结果
linux x64 CPU, clang 10.0
```
We have OpenMP 4.5.
_OPENMP is 201511
```

android arm64, ndk-r21b
```
We have OpenMP 3.1.
_OPENMP is 201107
```