#include "gft/test.hpp"
#include "gft/assert.hpp"
#include "gft/log.hpp"
#include <exception>


// Returns the factorial of n
int Factorial(int n)
{
    if (n == 1 || n == 2) return n;
    return Factorial(n - 1) + Factorial(n - 2);
}

L_TEST(equal)
{
    int p = 4;
    L_ASSERT(p == 4);
    L_ASSERT(p == 3);
}

L_TEST(factorial)
{
    L_ASSERT(Factorial(1) == 1);
    L_ASSERT(Factorial(2) == 2);
    L_ASSERT(Factorial(3) == 6);
    L_ASSERT(Factorial(8) == 40320);
}

int main()
{
    try {
        liong::test::TestRegistry::run_all();
    }
    catch (const std::exception& e)
    {
        L_ERROR(e.what());
    }
}