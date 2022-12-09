#include <stdlib.h>
#include <functional>
#include <string>
#include <map>

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
    MY_LOGE("%s:%d: Failure\nValue of: %s\n  Actual: false\nExpected: true\n", filename, linenumber, assertion);
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

#define MY_TEST(name) \
    void my_test_##name()

// Returns the factorial of n
int Factorial(int n)
{
    if (n == 1 || n == 2) return n;
    return Factorial(n - 1) + Factorial(n - 2);
}

MY_TEST(factorial)
{
    MY_EXPECT_TRUE(Factorial(1) == 1);
    MY_EXPECT_TRUE(Factorial(2) == 2);
    MY_EXPECT_TRUE(Factorial(3) == 6);
    MY_EXPECT_TRUE(Factorial(8) == 40320);
}

int main()
{
    my_test_factorial();
}