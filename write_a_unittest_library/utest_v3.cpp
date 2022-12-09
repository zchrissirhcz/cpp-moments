#if __ANDROID_API__ >= 8
#include <android/log.h>
#define MY_LOGE(...)                                                \
    do {                                                               \
        fprintf(stderr, ##__VA_ARGS__);                                \
        __android_log_print(ANDROID_LOG_WARN, "my", ##__VA_ARGS__); \
    } while (0)
#else
#include <stdio.h>
#define MY_LOGE(...)                 \
    do {                                \
        fprintf(stderr, ##__VA_ARGS__); \
    } while (0)
#endif

void my_assert_fail(const char* assertion, const char* filename, unsigned int linenumber, const char* function)
{
    MY_LOGE("%s:%d %s(): Assertion `%s' failed.", filename, linenumber, function, assertion);
}

#define MY_ASSERT(expr) \
    (static_cast<bool>(expr)    \
    ? void(0) \
    : my_assert_fail(#expr, __FILE__, __LINE__, __FUNCTION__))


int main()
{
    int p = 4;
    MY_ASSERT(p == 3);
    MY_ASSERT(p == 4);
    return 0;
}