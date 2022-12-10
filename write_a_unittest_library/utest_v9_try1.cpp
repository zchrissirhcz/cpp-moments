#include <stdio.h>

class Entity
{
private:
    Entity() { };
    ~Entity() { };
public:
    Entity(const Entity& other) = delete;
    Entity operator=(const Entity& other) = delete;
    
    static Entity& get_instance()
    {
        static Entity entity;
        return entity;
    }
    int hello()
    {
        printf("hello, entity()\n");
        return 0;
    }
};

int g1 = Entity::get_instance().hello();
int g2 = Entity::get_instance().hello();

int main()
{
    printf("hello main\n");
    return 0;
}