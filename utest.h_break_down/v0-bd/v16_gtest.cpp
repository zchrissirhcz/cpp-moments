#include <stdio.h>
#include "gtest/gtest.h"

TEST(c, 21)
{
    EXPECT_EQ(1, 1);
    EXPECT_EQ(1+2, 1+3);
    EXPECT_EQ(1, 2);
    EXPECT_EQ(2, 2);
}

TEST(c, 22)
{
    ASSERT_EQ(1, 1);
    ASSERT_EQ(2, 2);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}