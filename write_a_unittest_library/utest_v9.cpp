#include <stdlib.h>
#include <functional>
#include <string>
#include <map>
#include <vector>

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

#define MY_ESCAPE_COLOR_RED "\x1b[31m"
#define MY_ESCAPE_COLOR_GREEN "\x1b[32m"
#define MY_ESCAPE_COLOR_YELLOW "\x1b[33m"
#define MY_ESCAPE_COLOR_END "\x1b[0m"

void my_true_fail(const char* assertion, const char* filename, unsigned int linenumber, const char* function)
{
    MY_LOGE("%s%s:%d: Failure%s\n", MY_ESCAPE_COLOR_RED, filename, linenumber, MY_ESCAPE_COLOR_END);
    MY_LOGE("Value of: %s\n  Actual: false\nExpected: true\n", assertion);
}

void my_true_success(const char* assertion, const char* filename, unsigned int linenumber, const char* function)
{
    MY_LOGE("%s:%d %s(): Assertion %s`%s' ok%s.\n", filename, linenumber, function, MY_ESCAPE_COLOR_GREEN, assertion, MY_ESCAPE_COLOR_END);
}

void my_equal_fail(const char* expected, const char* actual, const char* filename, unsigned int linenumber, const char* function)
{
    MY_LOGE("%s%s:%d: Failure%s\n", MY_ESCAPE_COLOR_RED, filename, linenumber, MY_ESCAPE_COLOR_END);
    MY_LOGE("Expected: %s\n  Actual: %s\nExpected: true\n", expected, actual);
}

void my_equal_success(const char* expected, const char* actual, const char* filename, unsigned int linenumber, const char* function)
{
    MY_LOGE("%s:%d %s(): %s`MY_EXPECTED_EQ(%s, %s)' ok%s.\n", filename, linenumber, function, MY_ESCAPE_COLOR_GREEN, expected, actual, MY_ESCAPE_COLOR_END);
}

#define MY_EXPECT_TRUE(expr) \
    (static_cast<bool>(expr)    \
    ? my_true_success(#expr, __FILE__, __LINE__, __FUNCTION__) \
    : my_true_fail(#expr, __FILE__, __LINE__, __FUNCTION__))

#define MY_ASSERT_TRUE(expr) \
    if (static_cast<bool>(expr)) {   \
        my_true_success(#expr, __FILE__, __LINE__, __FUNCTION__); \
    } \
    else { \
        my_true_fail(#expr, __FILE__, __LINE__, __FUNCTION__); \
        return; \
    }

#define MY_EXPECT_EQ(expected, actual) \
    (static_cast<bool>(expected == actual)  \
    ? my_equal_success(#expected, #actual, __FILE__, __LINE__, __FUNCTION__) \
    : my_equal_fail(#expected, #actual, __FILE__, __LINE__, __FUNCTION__));

#define MY_ASSERT_EQ(expected, actual) \
    if (static_cast<bool>(expected == actual)) {    \
        my_equal_success(#expected, #actual, __FILE__, __LINE__, __FUNCTION__); \
    } \
    else { \
        my_equal_fail(#expected, #actual, __FILE__, __LINE__, __FUNCTION__); \
        return; \
    }

class TestEntity
{
private:
    TestEntity() { };
    ~TestEntity() { };
public:
    TestEntity(const TestEntity& other) = delete;
    TestEntity operator=(const TestEntity& other) = delete;
    
    static TestEntity& get_instance()
    {
        static TestEntity entity;
        return entity;
    }
    int add(std::function<void()> f)
    {
        test_funcs.push_back(f);
        return 0;
    }
    int run_all_test_functions()
    {
        for (auto f : test_funcs)
        {
            f();
        }
        return 0;
    }
private:
    std::vector<std::function<void()>> test_funcs;
};

#define MY_TEST(name) \
    void my_test_##name(); \
    int my_test_mark_##name = TestEntity::get_instance().add(my_test_##name); \
    void my_test_##name()

#define MY_UNUSED(x) (void)(x)

#define MY_RUN_ALL_TESTS() \
    int my_test_invoker = TestEntity::get_instance().run_all_test_functions(); \
    MY_UNUSED(my_test_invoker)

// Returns the factorial of n
int Factorial(int n)
{
    if (n == 1 || n == 2) return n;
    return Factorial(n - 1) + Factorial(n - 2);
}

MY_TEST(equal)
{
    int p = 3;
    MY_EXPECT_TRUE(p == 3);
}

MY_TEST(factorial)
{
    // MY_EXPECT_EQ(Factorial(1), 1);
    // MY_EXPECT_EQ(Factorial(2), 2);
    // MY_EXPECT_EQ(Factorial(3), 6);
    // MY_EXPECT_EQ(Factorial(8), 40320);

    // MY_ASSERT_EQ(Factorial(1), 1);
    // MY_ASSERT_EQ(Factorial(2), 2);
    // MY_ASSERT_EQ(Factorial(3), 6);
    // MY_ASSERT_EQ(Factorial(8), 40320);

    // MY_ASSERT_TRUE(Factorial(1) == 1);
    // MY_ASSERT_TRUE(Factorial(2) == 2);
    // MY_ASSERT_TRUE(Factorial(3) == 6);
    // MY_ASSERT_TRUE(Factorial(8) == 40320);

    MY_EXPECT_TRUE(Factorial(1) == 1);
    MY_EXPECT_TRUE(Factorial(2) == 2);
    MY_EXPECT_TRUE(Factorial(3) == 6);
    MY_EXPECT_TRUE(Factorial(8) == 40320);
}

int main()
{
    MY_RUN_ALL_TESTS();
    return 0;
}