#include <stdio.h>

#define TEST(set, name) \
    void qtest_##set##_##name()

#define EXPECT_EQ(x, y)

TEST(c, 1)
{
    EXPECT_EQ(1, 1);
}

TEST(c, 2)
{
    EXPECT_EQ(2, 2);
}

void qtest_init()
{
    printf("[==========] Running 2 tests from 1 test suite.\n");
    printf("[----------] Global test environment set-up.\n");
    printf("[----------] 2 tests from c\n");
}

int RUN_ALL_TESTS()
{
    printf("[ RUN      ] c.1\n");
    qtest_c_1();
    printf("[       OK ] c.1 (0 ms)\n");

    printf("[ RUN      ] c.2\n");
    qtest_c_2();
    printf("[       OK ] c.2 (0 ms)\n");
    printf("[----------] 2 tests from c (0 ms total)\n");

    printf("\n");
    printf("[----------] Global test environment tear-down\n");
    printf("[==========] 2 tests from 1 test suite ran. (0 ms total)\n");
    printf("[  PASSED  ] 2 tests.\n");

    return 0;
}

int main()
{
    qtest_init();
    return RUN_ALL_TESTS();
}