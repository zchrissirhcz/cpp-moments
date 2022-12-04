## Python JIT: numba, taichi 的简单使用以及和手写优化的比较

## 0x1 目的 && 环境

**目的**

对于 Python JIT 一无所知， 初步查到 numba 和 taichi 是两个知名的实现， 打算比对 numba， taichi 以及手写 C++ 和手动优化这几种方式的耗时。

选定的任务是对于 NxN 的矩阵每个元素计算 tanh 后再累加求和。 Python 中数据类型用的 double。 C++ 出于 SIMD 指令的考虑使用的是 float 类型（0~1之间），N=2000.


**环境**

- Ubuntu 20.04 x86_64
- clang-10 (via apt), LLVM-10 (from taichi website)
- Python 3.10.6
- taichi: 1.2.0
- numba: 0.56.3
- OpenCV(C++): 4.2 (via apt-get)

```bash
conda update conda
conda update python
python -m pip install -U taichi
python -m pip install -U numba
sudo apt install libopencv-dev
cd ~/work/taichi
export CXX=/usr/bin/clang++-10
export CC=/usr/bin/clang-10 
python3 setup.py install   
```

运行 main.py 得到 Python 结果。
运行 make 和 ./a.out 得到 C++ 的结果。

## 0x2 C++ 生成和 Python 一样的随机数
为了确保计算结果正确性， 打算让 Python 和 C++ 代码使用同样的随机输入。除了固定随机数种子， 还发现 numpy 的随机数是 C++ 的随机数的每2个取1个， 于是让 C++ 生成随机数的过程中每次 skip 掉一个再保留一个。

```Python
import numpy as np

def demo_random_number():
    # 生成10个在[0.0, 1.0)区间的随机数并打印。种子是固定的.
    seed = 7767517
    np.random.seed(seed)
    size = 10
    x = np.random.random(size)
    print(x)
```

基于 C++11 的实现， 需要每生成两个随机数时丢掉第二个，只使用第一个， 这样做才能和 numpy 的结果一致：
( ref: https://stackoverflow.com/questions/65871948/same-random-numbers-in-c-as-computed-by-python3-numpy-random-rand )
```c++
#include <stdio.h>
#include <random>

int main()
{
    int seed = 7767517;
    std::mt19937 random_generator(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int size = 10;
    int bufsize = size * size * sizeof(float);
    float* data = (float*)malloc(bufsize);
    for (int i = 0; i < size; i++)
    {
        float v1 = dist(random_generator);
        dist(random_generator); // we have to skip the second random number, to match numpy's result
        data[i] = v1;
    }

    for (int i = 0; i < size; i++)
    {
        if (i > 0)
        {
            printf(", ");
        }
        printf("%.6f", data[i]);
    }
    std::cout << std::endl;

    return 0;
}
```

C++ 的输出：
```
0.915606, 0.436727, 0.786953, 0.789857, 0.645815, 0.631617, 0.305545, 0.406294, 0.794875, 0.723695
```

Python 的输出:
```
[0.91560644 0.4367269  0.78695306 0.78985717 0.64581455 0.63161738
 0.30554508 0.40629352 0.79487504 0.7236945 ]
```

## 0x3 Python JIT: numba 和 taichi 的结果
```python
# numba 实现
@jit
def tan_sum_jit(a, size):   # 函数在被调用时编译成机器语言
    tan_sum = 0
    for i in range(size):   # Numba 支持循环
        for j in range(size):
            tan_sum += np.tanh(a[i, j])   # Numba 支持绝大多数 NumPy 函数
    return tan_sum

# naive 实现
def tan_sum_naive(a, size):
    tan_sum = 0
    for i in range(size):
        for j in range(size):
            tan_sum += np.tanh(a[i, j])
    return tan_sum

# taichi 实现
@ti.kernel
def tan_sum_taichi(a: ti.types.ndarray(), size: int) -> ti.f64:
    tan_sum: ti.f64 = 0.0
    for i in range(size):
        for j in range(size):
            tan_sum += ti.tanh(a[i, j])
    return tan_sum
```

PC耗时 (Linux x64)

| 方法                   | 耗时           |
| ---------------------- | -------------- |
| numba 实现(第1次运行)  | 1190.552473 ms |
| numba 实现(第2次运行)  | 244.834423 ms  |
| naive 实现             | 2929.605246 ms |
| naive 实现             | 2918.185472 ms |
| taichi 实现(第1次运行) | 70.072651 ms   |
| taichi 实现(第2次运行) | 46.470642 ms   |


## 0x4 C++ 耗时一览: on PC
C++ 的实现中， 用到了 SIMD 指令。 由于本人对 SSE 完全不会， 于是按 ARM NEON 的指令编写， 用 neon2sse.h 进行翻译。 此外还用到了 tanh 的 neon 实现 （注意它不是 neon intrinsic 自带的， 是从开源项目 ncnn 中移植过来的）。 此外还使用了开源项目 OpenCV 中的多线程接口 `cv::parallel_for_` 做多线程加速。

稍作实现， 发现单纯用 SIMD 加速（SSE2指令）就达到了最短的耗时（本人公司电脑老旧，不支持 avx）; 多线程实现也比 naive 实现有明显加速（opencv 在 PC 上默认用4线程）， 不过多线程 + simd 的双重循环实现则没有太明显耗时变化， 猜测是 O2 编译优化选项自动做了 SIMD：

PC耗时 (Linux x64, C++)
| 方法                 | 耗时      |
| -------------------- | --------- |
| naive c++            | 67.287 ms |
| one_loop c++         | 68.183 ms |
| simd c++             |  4.233 ms |
| multithread c++      | 15.778 ms |
| simd_multithread c++ | 14.480 ms |

## 0x5 C++ 耗时一览： on Android

使用的设备是 小米 11， 绑定小核心(Cortext A55).

多线程用 opencv 默认的，是 2 线程。耗时如下：

Android (小米11, 绑小核到Cortex-A55)
| 方法                 | 耗时       |
| -------------------- | ---------- |
| naive c++            | 423.805 ms |
| one_loop c++         | 425.243 ms |
| simd c++             |  48.798 ms |
| multithread c++      | 152.024 ms |
| simd_multithread c++ | 136.163 ms |

## 0x6. AOT on PC(Linux)

### 6.1 编译安装 taichi
official steps: https://docs.taichi-lang.org/docs/dev_install

首先准备 cmake， python（基于 miniconda）

需要使用 clang-10. 高版本的 clang-14 不被支持。
```bash
sudo apt install libclang-10-dev
sudo apt install clang-10

cd ~/work
git clone https://github.com/taichi-dev/taichi
cd taichi
git submodule update --init --recursive

python3 -m pip install --user -r requirements_dev.txt

source ~/soft/vulkansdk/1.3.204.1/setup-env.sh
export TAICHI_CMAKE_ARGS="-DCMAKE_CXX_COMPILER=/usr/bin/clang++-10 -DCMAKE_C_COMPILER=/usr/bin/clang-10 -DTI_WITH_VULKAN=ON -DTI_WITH_LLVM=OFF -DTI_WITH_METAL=OFF -DTI_WITH_C_API=ON $TAICHI_CMAKE_ARGS"
python setup.py install
```

其中 C_API 会安装到 `${TAICHI_ROOT}/_skbuild` 目录里的子目录中. 比如我是 `/home/zz/work/taichi/_skbuild/linux-x86_64-3.10/cmake-install/c_api`.

### 6.2 编译 taichi-aot-demo
```bash
cd ~/work
git clone https://github.com/taichi-dev/taichi-aot-demo
cd taichi-aot-demo
git submodule update --init --recursive

export CC=/usr/bin/clang-10
export CXX=/usr/bin/clang++-10 
export TAICHI_C_API_INSTALL_DIR=/home/zz/work/taichi/_skbuild/linux-x86_64-3.10/cmake-install/c_api

cmake -S . -B build3 
cmake --build build3 -j4
```

## 7. References
- taichi
- numba
- ncnn
- OpenCV
- neon2sse