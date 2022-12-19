#include <stdio.h>
//#include "../v0/test/utest.h"
#include "utest.h"

//#define TEST TESTCASE
#define TEST UTEST

TEST(c, 1)
{
    EXPECT_EQ(1, 1);
}
TEST(c, 2)
{
    EXPECT_EQ(2, 3);
}

TEST(d, 1)
{
    EXPECT_NE(2, 3);
}
TEST(d, 2)
{
    EXPECT_NE(2, 2);
}

TEST(e, 1)
{
    EXPECT_LE(2, 3);
}
TEST(e, 2)
{
    EXPECT_LE(2, 2);
}

// int main()
// {
//     testing::InitGoogleTest();
//     return RUN_ALL_TESTS();
// }
UTEST_MAIN()