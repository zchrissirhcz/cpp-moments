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

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}