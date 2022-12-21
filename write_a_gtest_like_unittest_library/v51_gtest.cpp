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

TEST(cd, 1)
{
    EXPECT_NE(2, 3);
}
TEST(cd, 2)
{
    EXPECT_NE(2, 2);
}

TEST(ce, 1)
{
    EXPECT_LE(2, 3);
}
TEST(ce, 2)
{
    EXPECT_LE(2, 2);
}

TEST(f, 1)
{
    EXPECT_LT(2, 5);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    // Run a specific test only
    //testing::GTEST_FLAG(filter) = "MyLibrary.TestReading"; // I'm testing a new feature, run something quickly
    testing::GTEST_FLAG(filter) = "c*.1";

    // Exclude a specific test
    //testing::GTEST_FLAG(filter) = "-cvtColorTwoPlane.yuv420sp_to_rgb:-cvtColorTwoPlane.rgb_to_yuv420sp"; // The writing test is broken, so skip it

    return RUN_ALL_TESTS();
}