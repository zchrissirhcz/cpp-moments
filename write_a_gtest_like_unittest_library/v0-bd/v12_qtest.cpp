#include <stdio.h>
#include <stdlib.h>

bool qtest_success = true;
int qtest_total_cnt = 0;
int qtest_fail_cnt = 0;

#define TEST(set, name) \
    void qtest_##set##_##name()

#define EXPECT_EQ(x, y) \
    if ((x)!=(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Expected equality of these values:\n"); \
        printf("  %s\n  %s\n", #x, #y); \
        qtest_success = false; \
    } \
    else \
    { \
        qtest_success = true; \
    }

TEST(c, 1)
{
    EXPECT_EQ(1, 1);
}

TEST(c, 2)
{
    EXPECT_EQ(2, 3);
}

void qtest_init()
{
    printf("[==========] Running 2 tests from 1 test suite.\n");
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
    qtest_state_t qtest_state[2];

    // testcase1
    qtest_state[qtest_total_cnt].name = "c.1";
    printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name);
    qtest_c_1();
    if (qtest_success)
    {
        printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name);
    }
    else
    {
        printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name);
        qtest_fail_cnt++;
    }
    qtest_state[qtest_total_cnt].state = qtest_success;
    qtest_total_cnt++;

    // testcase2
    qtest_state[qtest_total_cnt].name = "c.2";
    printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name);
    qtest_c_2();
    if (qtest_success)
    {
        printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name);
    }
    else
    {
        printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name);
        qtest_fail_cnt++;
    }
    qtest_state[qtest_total_cnt].state = qtest_success;
    qtest_total_cnt++;

    // summary for test set `c`
    printf("[----------] %d tests from c (0 ms total)\n", qtest_total_cnt);

    printf("\n");
    printf("[----------] Global test environment tear-down\n");
    printf("[==========] 2 tests from 1 test suite ran. (0 ms total)\n");
    printf("[  PASSED  ] %d tests.\n", qtest_total_cnt - qtest_fail_cnt);
    if (qtest_fail_cnt)
    {
        printf("[  FAILED  ] %d test, listed below:\n", qtest_fail_cnt);
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
        printf("\n  %d FAILED TEST\n", qtest_fail_cnt);
    }

    return 0;
}

int main()
{
    qtest_init();
    return RUN_ALL_TESTS();
}