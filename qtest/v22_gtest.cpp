#include <stdio.h>
#include "gtest/gtest.h"

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

TEST(f, 1)
{
    EXPECT_LT(2, 5);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}