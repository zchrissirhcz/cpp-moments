#include <stdio.h>
#include <string>
#include <vector>

class Entity
{
private:
    Entity() { };
    ~Entity() {};
public:
    Entity(const Entity& other) = delete;
    Entity operator=(const Entity& other) = delete;
    
    static Entity& get_instance()
    {
        static Entity entity;
        return entity;
    }
    int hello(const std::string& name)
    {
        vs.push_back(name);
        return 0;
    }

    int print_all_names()
    {
        for (size_t i = 0; i < vs.size(); i++)
        {
            printf("%s\n", vs[i].c_str());
        }
        return 0;
    }

private:
    std::vector<std::string> vs;
};

int g1 = Entity::get_instance().hello("one");
int g2 = Entity::get_instance().hello("two");
int g3 = Entity::get_instance().print_all_names();

int main()
{
    printf("hello main\n");
    return 0;
}