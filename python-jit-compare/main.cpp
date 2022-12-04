#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <random>
#include <iostream>

#include "autotimer.hpp"
#include "cpu_helper.hpp"

#include "neon_mathfun.h"
#include "neon_mathfun_tanh.h"

#if __ARM_NEON
#include <arm_neon.h>
#else
#include "neon2sse.h"
#endif

#include <opencv2/opencv.hpp>

#include <atomic>

float tan_sum_naive(float* a, int size)
{
    float tan_sum = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            tan_sum += tanh(a[i * size + j]);
        }
    }
    return tan_sum;
}

float tan_sum_one_loop(float* a, int size)
{
    const int total = size * size;
    float tan_sum = 0;
    for (int i = 0; i < total; i++)
    {
        tan_sum += tanh(a[i]);
    }
    return tan_sum;
}

float tan_sum_simd(float* a, int size)
{
    const int total = size * size;
    float tan_sum = 0.f;
    float* sp = a;

#if __ARM_NEON
    int nn = total >> 2;
    int remain = total - (nn << 2);
#else
    int remain = total;
#endif // __ARM_NEON

#if __ARM_NEON
    float32x4_t v_sum = vdupq_n_f32(0);
    for (int i = 0; i < nn; i++)
    {
        float32x4_t v_sp = vld1q_f32(sp);
        float32x4_t v_d = tanh_ps(v_sp);
        v_sum = vaddq_f32(v_sum, v_d);
        sp += 4;
    }
    float sum_lst[4];
    vst1q_f32(sum_lst, v_sum);
    tan_sum = sum_lst[0] + sum_lst[1] + sum_lst[2] + sum_lst[3];
#endif // __ARM_NEON

    for (; remain > 0; remain--)
    {
        tan_sum += tanh(*sp);
        sp++;
    }

    return tan_sum;
}

float tan_sum_multithread(float* a, int size)
{
    float tan_sum = 0;

    std::atomic<float> sum_atomic(0);
    cv::parallel_for_(cv::Range(0, size), [&](const cv::Range& range) {
        float sum = 0;
        for (int i = range.start; i < range.end; i++)
        {
            float* sp = a + i * size;
            for (int j = 0; j < size; j++)
            {
                sum += tanh(sp[j]);
            }
        }
        sum_atomic = sum_atomic + sum;
    });
    
    tan_sum = sum_atomic;

    return tan_sum;
}

float tan_sum_simd_multithread(float* a, int size)
{
    float tan_sum = 0;

    std::atomic<float> sum_atomic(0);
    cv::parallel_for_(cv::Range(0, size), [&](const cv::Range& range) {
        float sum = 0;
        for (int i = range.start; i < range.end; i++)
        {
            float* sp = a + i * size;
            
#if __ARM_NEON
            int nn = size >> 2;
            int remain = size - (nn << 2);
#else
            int remain = size;
#endif // __ARM_NEON

#if __ARM_NEON
            float32x4_t v_sum = vdupq_n_f32(0);
            for (int j = 0; j < nn; j++)
            {
                float32x4_t v_sp = vld1q_f32(sp);
                float32x4_t v_d = tanh_ps(v_sp);
                v_sum = vaddq_f32(v_sum, v_d);
                sp += 4;
            }
            float sum_lst[4];
            vst1q_f32(sum_lst, v_sum);
            sum += (sum_lst[0] + sum_lst[1] + sum_lst[2] + sum_lst[3]);
#endif // __ARM_NEON

            for (; remain > 0; remain--)
            {
                sum += tanh(*sp);
                sp++;
            }
        }
        sum_atomic = sum_atomic + sum;
    });
    
    tan_sum = sum_atomic;

    return tan_sum;
}

void example_generate_random_nubmer_same_as_python()
{
    std::mt19937 random_generator(7767517);
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
}


float* prepare_NxN_float_data(int size)
{
    std::mt19937 random_generator(7767517);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    int bufsize = size * size * sizeof(float);
    float* data = (float*)malloc(bufsize);
    int total_num = size * size;
    for (int i = 0; i < total_num; i++)
    {
        float v1 = dist(random_generator);
        dist(random_generator); // we have to skip the second random number, to match numpy's result
        data[i] = v1;
    }
    return data;
}

int main()
{
#if __ANDROID__
    plain::set_cpu_powersave(1);
#endif // __ANDROID__

    int size = 2000;
    float* data = prepare_NxN_float_data(size);
    // for (int i = 0; i < size * size; i++)
    // {
    //     if (i > 0)
    //     {
    //         printf(", ");
    //     }
    //     printf("%.6f", data[i]);
    // }
    // printf("\n");

    float result_naive;
    {
        AutoTimer timer("naive c++");
        result_naive = tan_sum_naive(data, size);
    }
    printf("naive result is %.6f\n", result_naive);

    float result_one_loop;
    {
        AutoTimer timer("one_loop c++");
        result_one_loop = tan_sum_naive(data, size);
    }
    printf("one_loop result is %.6f\n", result_one_loop);

    float result_simd;
    {
        AutoTimer timer("simd c++");
        result_simd = tan_sum_simd(data, size);
    }
    printf("simd result is %.6f\n", result_simd);

    float result_multithread;
    {
        AutoTimer timer("multithread c++");
        result_multithread = tan_sum_multithread(data, size);
    }
    printf("multithread result is %.6f\n", result_multithread);

    float result_simd_multithread;
    {
        AutoTimer timer("simd_multithread c++");
        result_simd_multithread = tan_sum_multithread(data, size);
    }
    printf("simd_multithread result is %.6f\n", result_simd_multithread);

    free(data);

    return 0;
}