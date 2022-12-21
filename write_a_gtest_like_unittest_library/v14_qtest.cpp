#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool qtest_success = true;
int qtest_total_cnt = 0;
int qtest_fail_cnt = 0;
const int qtest_test_total = 12;

#define TEST(set, name) \
    void qtest_##set##_##name()

#define QTEST_EXPECT(x, y, cond) \
    if (!((x)cond(y))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        if (strcmp(#cond, "==") == 0) \
        { \
            printf("Expected equality of these values:\n"); \
            printf("  %s\n  %s\n", #x, #y); \
        } \
        else \
        { \
            printf("Expected: (%s) %s (%s), actual: %s vs %s\n", #x, #cond, #y, #x, #y); \
        } \
        qtest_success = false; \
    } \
    else \
    { \
        qtest_success = true; \
    }

#define EXPECT_EQ(x, y) QTEST_EXPECT(x, y, ==)
#define EXPECT_NE(x, y) QTEST_EXPECT(x, y, !=)
#define EXPECT_LT(x, y) QTEST_EXPECT(x, y, <)
#define EXPECT_LE(x, y) QTEST_EXPECT(x, y, <=)
#define EXPECT_GT(x, y) QTEST_EXPECT(x, y, >)
#define EXPECT_GE(x, y) QTEST_EXPECT(x, y, >=)

TEST(c, 1)
{
    EXPECT_EQ(1, 1);
}
TEST(c, 2)
{
    EXPECT_EQ(2, 3);
}

TEST(c, 3)
{
    EXPECT_NE(2, 3);
}
TEST(c, 4)
{
    EXPECT_NE(2, 2);
}

TEST(c, 5)
{
    EXPECT_LT(2, 3);
}
TEST(c, 6)
{
    EXPECT_LT(2, 2);
}

TEST(c, 7)
{
    EXPECT_LE(2, 3);
}
TEST(c, 8)
{
    EXPECT_LE(2, 1);
}

TEST(c, 9)
{
    EXPECT_GT(2, 1);
}
TEST(c, 10)
{
    EXPECT_GT(2, 2);
}

TEST(c, 11)
{
    EXPECT_GE(2, 1);
}
TEST(c, 12)
{
    EXPECT_GT(2, 2);
}

void qtest_init()
{
    printf("[==========] Running %d tests from 1 test suite.\n", qtest_test_total);
    printf("[----------] Global test environment set-up.\n");
    printf("[----------] 2 tests from c\n");
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
        printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name); \
        qtest_##test_set##_##test_name(); \
        if (qtest_success) \
        { \
            printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
        } \
        else \
        { \
            printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
            qtest_fail_cnt++; \
        } \
        qtest_state[qtest_total_cnt].state = qtest_success; \
        qtest_total_cnt++; \
    }

    // testcase1
    RUN_SINGLE_TEST(c, 1);
    RUN_SINGLE_TEST(c, 2);
    RUN_SINGLE_TEST(c, 3);
    RUN_SINGLE_TEST(c, 4);
    RUN_SINGLE_TEST(c, 5);
    RUN_SINGLE_TEST(c, 6);
    RUN_SINGLE_TEST(c, 7);
    RUN_SINGLE_TEST(c, 8);
    RUN_SINGLE_TEST(c, 9);
    RUN_SINGLE_TEST(c, 10);
    RUN_SINGLE_TEST(c, 11);
    RUN_SINGLE_TEST(c, 12);

    // summary for test set `c`
    printf("[----------] %d tests from c (0 ms total)\n", qtest_total_cnt);

    printf("\n");
    printf("[----------] Global test environment tear-down\n");
    printf("[==========] %d tests from 1 test suite ran. (0 ms total)\n", qtest_total_cnt);
    printf("[  PASSED  ] %d tests.\n", qtest_total_cnt - qtest_fail_cnt);
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