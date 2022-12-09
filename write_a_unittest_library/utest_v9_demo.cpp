#include <stdio.h>
#include <functional>

void hello()
{
    printf("hello world\n");
}

int main()
{
    std::function<void()> f = hello;
    f();
    return 0;
}