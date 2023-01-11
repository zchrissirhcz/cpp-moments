#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char** names;

    names = (char**)malloc(sizeof(char**) * 1);
    names[0] = "hello";

    return 0;
}
