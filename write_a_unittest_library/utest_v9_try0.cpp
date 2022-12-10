#include <stdio.h>
#include <functional>

class Entity
{
public:
    Entity()
    {
        printf("Entity constructor\n");
    }
};

Entity entity;

int main()
{
    printf("hello, main()\n");

    return 0;
}