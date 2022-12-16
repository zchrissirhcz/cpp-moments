#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

// casting for expression
#ifdef __cplusplus
#define QTEST_CAST(typ, x) static_cast<type>(x)
#define QTEST_PTR_CAST(type, x) reinterpret_cast<type>(x)
#else
#define QTEST_CAST(type, x) ((type)x)
#define QUTEST_PTR_CAST(type, x) ((type)x)
#endif

// 每个测试用例对应一个函数， 函数类型是 qtest_testcase_t
typedef void(*qtest_testcase_t)(int*);

typedef struct qtest_state_s
{
    qtest_testcase_t* testcases;
    const char** testcase_names;
    size_t testcases_length;
} qtest_state_s;

struct qtest_state_s qtest_state;

#define QTEST_ASSERT(x, y, cond) \
    if (!(x)cond(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        return; \
    }

#define ASSERT_EQ(x, y) QTEST_ASSERT(x, y, ==)
#define ASSERT_NE(x, y) QTEST_ASSERT(x, y, !=)
#define ASSERT_LT(x, y) QTEST_ASSERT(x, y, <)
#define ASSERT_LE(x, y) QTEST_ASSERT(x, y, <=)
#define ASSERT_GT(x, y) QTEST_ASSERT(x, y, >)
#define ASSERT_GE(x, y) QTEST_ASSERT(x, y, >=)

#define ASSERT_TRUE(x) \
    if (x) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        return; \
    }

#define ASSERT_FALSE(x) \
    if (x) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        return; \
    }

// EXPECT_* 系列
#define EXPECT_TRUE(x) \
    if (!(x)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
    }

#define EXPECT_FALSE(x) \
    if (x) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
    }

#define QTEST_EXPECT(x, y, cond) \
    if (!(x)cond(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
    }

#define EXPECT_EQ(x, y) QTEST_EXPECT(x, y, ==)
#define EXPECT_NE(x, y) QTEST_EXPECT(x, y, !=)
#define EXPECT_LT(x, y) QTEST_EXPECT(x, y, <)
#define EXPECT_LE(x, y) QTEST_EXPECT(x, y, <=)
#define EXPECT_GT(x, y) QTEST_EXPECT(x, y, >)
#define EXPECT_GE(x, y) QTEST_EXPECT(x, y, >=)

// 首先声明函数， 然后注册函数， 最后是定义函数(只写出函数头)
//qtest_state.testcases = (qtest_testcase_t*)malloc(sizeof(qtest_testcase_t));
#define TEST(set, name) \
    static void qtest_##set##_##name(); \
    void qtest_##set##_##name()

TEST(c, 1)
{
    ASSERT_EQ(1, 1);
}

TEST(c, 2)
{
    ASSERT_EQ(2, 2);
}

int main()
{
    printf("[==========] Running %u test cases.\n",
        (unsigned)qtest_state.testcases_length
    );
    size_t index;
    for (index = 0; index < qtest_state.testcases_length; index++)
    {
        printf("[ Run      ] %s\n",
            qtest_state.testcase_names[index]
        );
    }
    return 0;
}