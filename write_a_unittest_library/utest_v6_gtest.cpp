#include "gtest/gtest.h"

// Returns the factorial of n
int Factorial(int n)
{
    if (n == 1 || n == 2) return n;
    return Factorial(n - 1) + Factorial(n - 2);
}

TEST(factorial, a)
{
    EXPECT_TRUE(Factorial(1) == 1);
    EXPECT_TRUE(Factorial(2) == 2);
    EXPECT_TRUE(Factorial(3) == 6);
    EXPECT_TRUE(Factorial(8) == 40320);
}
