#include <stdio.h>

int my_max(int x, int y)
{
    return x > y ? x : y;
}

typedef int(*max_fun)(int, int);

int main()
{
    max_fun max = my_max;

    int a = 3;
    int b = 4;
    int c = max(a, b);
    printf("c = %d\n", c);

    return 0;
}