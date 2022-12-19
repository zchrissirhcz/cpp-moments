#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>
#include <iostream>
#include <string>

int qtest_current_fail_cnt = 0; // number of failures in one test function
int qtest_total_cnt = 0;
int qtest_fail_cnt = 0; // number of failures in one test set
const int qtest_test_total = 3;

#define TEST(set, name) \
    void qtest_##set##_##name()

#define QTEST_EXPECT(x, y, cond) \
    if (!((x)cond(y))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        if (strcmp(#cond, "==") == 0) \
        { \
            printf("Expected equality of these values:\n"); \
            printf("  %s\n", #x); \
            qtest_evaluate_if_required(#x, x); \
            printf("  %s\n", #y); \
            qtest_evaluate_if_required(#y, y); \
        } \
        else \
        { \
            printf("Expected: (%s) %s (%s), actual: %s vs %s\n", #x, #cond, #y, std::to_string(x).c_str(), std::to_string(y).c_str()); \
        } \
        qtest_current_fail_cnt++; \
    }

#define EXPECT_EQ(x, y) QTEST_EXPECT(x, y, ==)
#define EXPECT_NE(x, y) QTEST_EXPECT(x, y, !=)
#define EXPECT_LT(x, y) QTEST_EXPECT(x, y, <)
#define EXPECT_LE(x, y) QTEST_EXPECT(x, y, <=)
#define EXPECT_GT(x, y) QTEST_EXPECT(x, y, >)
#define EXPECT_GE(x, y) QTEST_EXPECT(x, y, >=)

#define EXPECT_TRUE(x) \
    if (!((x))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Value of: %s\n", #x); \
        printf("  Actual: false\n"); \
        printf("Expected: true\n"); \
        qtest_current_fail_cnt++; \
    }

#define EXPECT_FALSE(x) \
    if (((x))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Value of: %s\n", #x); \
        printf("  Actual: true\n"); \
        printf("Expected: false\n"); \
        qtest_current_fail_cnt++; \
    }

template<typename T>
void qtest_evaluate_if_required(const char* str, T value)
{
    if (strcmp(str, std::to_string(value).c_str()) != 0)
    {
        std::cout << "    Which is: " << value << std::endl;
    }
}

#define ASSERT_EQ(x, y) \
    if ((x)!=(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Expected equality of these values:\n"); \
        printf("  %s\n", #x); \
        qtest_evaluate_if_required(#x, x); \
        printf("  %s\n", #y); \
        qtest_evaluate_if_required(#y, y); \
        qtest_current_fail_cnt++; \
        return; \
    }

TEST(c, 21)
{
    EXPECT_EQ(1, 1);
    EXPECT_EQ(1+2, 1+3);
    EXPECT_EQ(1, 2);
    EXPECT_EQ(2, 2);
    EXPECT_NE(1+2, 2+1);
}

TEST(c, 22)
{
    ASSERT_EQ(1, 1);
    ASSERT_EQ(2, 2);
    ASSERT_EQ(2, 3);
}

TEST(c, 23)
{
    ASSERT_EQ(1+2, 1+3);
}

void qtest_init()
{
    printf("[==========] Running %d tests from 1 test suite.\n", qtest_test_total);
    printf("[----------] Global test environment set-up.\n");
    printf("[----------] %d tests from c\n", qtest_test_total);
}

typedef struct qtest_state_t
{
    const char* name;
    bool state;
} qtest_state_t;

int RUN_ALL_TESTS()
{
    qtest_state_t qtest_state[qtest_test_total];

#define RUN_SINGLE_TEST(test_set, test_name) \
    { \
        qtest_state[qtest_total_cnt].name = #test_set "." #test_name; \
        qtest_current_fail_cnt = 0; \
        printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name); \
        qtest_##test_set##_##test_name(); \
        if (qtest_current_fail_cnt == 0) \
        { \
            printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
        } \
        else \
        { \
            printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
            qtest_fail_cnt++; \
        } \
        qtest_state[qtest_total_cnt].state = (qtest_current_fail_cnt == 0); \
        qtest_total_cnt++; \
    }

    // testcase1
    RUN_SINGLE_TEST(c, 21);
    RUN_SINGLE_TEST(c, 22);
    RUN_SINGLE_TEST(c, 23);

    // summary for test set `c`
    printf("[----------] %d tests from c (0 ms total)\n", qtest_total_cnt);

    printf("\n");
    printf("[----------] Global test environment tear-down\n");
    printf("[==========] %d tests from 1 test suite ran. (0 ms total)\n", qtest_total_cnt);

    int passed_test_count = qtest_total_cnt - qtest_fail_cnt;
    printf("[  PASSED  ] %d test", passed_test_count);
    if (passed_test_count != 1)
    {
        printf("s");
    }
    printf(".\n");

    if (qtest_fail_cnt)
    {
        printf("[  FAILED  ] %d test", qtest_fail_cnt);
        if (qtest_fail_cnt > 1)
        {
            printf("s");
        }
        printf(", listed below:\n");
        for (int i = 0; i < qtest_total_cnt; i++)
        {
            if (!qtest_state[i].state)
            {
                printf("[  FAILED  ] %s\n", qtest_state[i].name);
            }
        }
    }

    if (qtest_fail_cnt > 0)
    {
        printf("\n %d FAILED TEST", qtest_fail_cnt);
        if (qtest_fail_cnt > 1)
        {
            printf("S");
        }
        printf("\n");
    }

    return 0;
}

int main()
{
    qtest_init();
    return RUN_ALL_TESTS();
}