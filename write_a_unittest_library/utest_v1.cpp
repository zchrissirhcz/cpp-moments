#include <stdio.h>

#define MY_ASSERT(expr) \
    if (expr) \
    { \
        printf("Success\n"); \
    } \
    else \
    { \
        printf("Failed\n"); \
    }

int main()
{
    int p = 4;
    MY_ASSERT(p == 3);
    return 0;
}