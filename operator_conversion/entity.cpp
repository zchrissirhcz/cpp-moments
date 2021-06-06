#include <iostream>
#include <string>

class Entity
{
public:
    Entity(int a): age(a), name("unknown") {}
    Entity(const std::string& n): age(0), name(n) {}

private:
    int age;
    std::string name;
};

int main()
{
    Entity e1("Chris");

    //Entity e2 = "Chris";

    Entity e3(3.5);

    Entity e4 = 4;

    double q = 233.3;
    e1 = std::string("Chris");

    return 0;
}
