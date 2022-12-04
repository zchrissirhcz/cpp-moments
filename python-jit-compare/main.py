# coding: utf-8

from numba import jit
import numpy as np
from torch_helper import setup_seed
import time
import taichi as ti

ti.init(ti.cpu)

"""
给定 n*n 矩阵, 对矩阵的每个元素计算 tanh 值, 然后求和,
因为要循环矩阵中的每个元素， 计算复杂度为 n*n.
"""
@jit
def tan_sum_jit(a, size):   # 函数在被调用时编译成机器语言
    tan_sum = 0
    for i in range(size):   # Numba 支持循环
        for j in range(size):
            tan_sum += np.tanh(a[i, j])   # Numba 支持绝大多数 NumPy 函数
    return tan_sum

def tan_sum_naive(a, size): # N = 2000
    tan_sum = 0
    for i in range(size):
        for j in range(size):
            tan_sum += np.tanh(a[i, j])
    return tan_sum

@ti.kernel
def tan_sum_taichi(a: ti.types.ndarray(), size: int) -> ti.f64:
    tan_sum: ti.f64 = 0.0
    for i in range(size):
        for j in range(size):
            tan_sum += ti.tanh(a[i, j])
    return tan_sum

def benchmark_for_numba(x, size):
    start_time = time.time()
    print(tan_sum_jit(x, size))
    time_cost = time.time() - start_time
    print("time cost of numba: {:.6f} ms".format(time_cost * 1000))

def benchmark_for_naive(x, size):
    start_time = time.time()
    print(tan_sum_naive(x, size))
    time_cost = time.time() - start_time
    print("time cost of naive: {:.6f} ms".format(time_cost * 1000))

def benchmark_for_taichi(x, size):
    start_time = time.time()
    print(tan_sum_taichi(x, size))
    time_cost = time.time() - start_time
    print("time cost of taicih: {:.6f} ms".format(time_cost * 1000))

def benchmark():
    setup_seed(7767517)
    size = 2000
    x = np.random.random((size, size))

    benchmark_for_numba(x, size)
    benchmark_for_naive(x, size)
    benchmark_for_taichi(x, size)

def demo_random_number():
    #setup_seed(7767517)
    np.random.seed(7767517)
    size = 3
    x = np.random.random(size)
    print(x)

if __name__ == '__main__':
    benchmark()
    #demo_random_number()