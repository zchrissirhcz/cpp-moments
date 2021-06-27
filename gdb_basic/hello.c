#include <stdio.h>

int max(int a, int b)
{
    printf("--- in max()\n");
    if (a>b) {
        return a;
    } else {
        return b;
    }
}

void hello(int a)
{
    printf("--- in hello()\n");
    int b = 2*a - 15;
    int c = max(a, b);
    printf(">>> c is %d\n", c);
}

int main()
{
    printf("Hello world!\n");
    int a = 0;
    for (int i=0; i<10; i++) {
        a += i;
    }
    hello(a);
    printf("a = %d\n", a);
    

    return 0;
}