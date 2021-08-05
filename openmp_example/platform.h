#pragma once

#define NCNN_STDIO 1

#if NCNN_STDIO
#if NCNN_PLATFORM_API && __ANDROID_API__ >= 8
#include <android/log.h>
#define NCNN_LOGE(...) do { \
    fprintf(stderr, ##__VA_ARGS__); fprintf(stderr, "\n"); \
    __android_log_print(ANDROID_LOG_WARN, "ncnn", ##__VA_ARGS__); } while(0)
#else // NCNN_PLATFORM_API && __ANDROID_API__ >= 8
#include <stdio.h>
#define NCNN_LOGE(...) do { \
    fprintf(stderr, ##__VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#endif // NCNN_PLATFORM_API && __ANDROID_API__ >= 8
#else
#define NCNN_LOGE(...)
#endif