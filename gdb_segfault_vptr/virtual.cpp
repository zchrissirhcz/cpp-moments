#include <iostream>

class Base
{
public:
    virtual int* hello() {
        printf("Base::hello()\n");
        int* data = new int[10];
        return data;
    }
    virtual ~Base() {}
private:
    int len;
};

class Derived: public Base
{
public:
    int* hello() {
        printf("Derived::hello()\n");
        return NULL;
    }
    ~Derived() {}
};

void play(int a, Base* ptr, int& c)
{
    c = *(ptr->hello());
}

int main() {
    int a = 1;
    Base* ptr = new Derived;
    int c;
    play(a, ptr, c);

    return 0;
}
