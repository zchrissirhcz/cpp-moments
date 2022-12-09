#include <stdlib.h>

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
    MY_LOGE("%s:%d %s(): Assertion `%s' failed.\n", filename, linenumber, function, assertion);
}

void my_assert_success(const char* assertion, const char* filename, unsigned int linenumber, const char* function)
{
    MY_LOGE("%s:%d %s(): Assertion `%s' ok.\n", filename, linenumber, function, assertion);
}

#define MY_EXPECT_TRUE(expr) \
    (static_cast<bool>(expr)    \
    ? my_assert_success(#expr, __FILE__, __LINE__, __FUNCTION__) \
    : my_assert_fail(#expr, __FILE__, __LINE__, __FUNCTION__))

#define MY_ASSERT_TRUE(expr) \
    (static_cast<bool>(expr)    \
    ? my_assert_success(#expr, __FILE__, __LINE__, __FUNCTION__) \
    : my_assert_fail(#expr, __FILE__, __LINE__, __FUNCTION__)); return;

void test_sample()
{
    int p = 4;
    MY_EXPECT_TRUE(p == 3);
    MY_EXPECT_TRUE(p == 4);

    MY_ASSERT_TRUE(p == 3);
    MY_ASSERT_TRUE(p == 4);
}

int main()
{
    test_sample();
    return 0;
}