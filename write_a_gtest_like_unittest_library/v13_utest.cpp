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

TEST(c, 3)
{
    EXPECT_NE(2, 3);
}
TEST(c, 4)
{
    EXPECT_NE(2, 2);
}

// TEST(c, 5)
// {
//     EXPECT_LT(2, 3);
// }
// TEST(c, 6)
// {
//     EXPECT_LT(2, 2);
// }

// TEST(c, 7)
// {
//     EXPECT_LE(2, 3);
// }
// TEST(c, 8)
// {
//     EXPECT_LE(2, 1);
// }

// TEST(c, 9)
// {
//     EXPECT_GT(2, 1);
// }
// TEST(c, 10)
// {
//     EXPECT_GT(2, 2);
// }

// TEST(c, 11)
// {
//     EXPECT_GE(2, 1);
// }
// TEST(c, 12)
// {
//     EXPECT_GT(2, 2);
// }

// TEST(c, 13)
// {
//     EXPECT_TRUE(1);
// }
// TEST(c, 14)
// {
//     EXPECT_TRUE(0);
// }

// TEST(c, 15)
// {
//     EXPECT_FALSE(0);
// }
// TEST(c, 16)
// {
//     EXPECT_FALSE(1);
// }

UTEST_MAIN()