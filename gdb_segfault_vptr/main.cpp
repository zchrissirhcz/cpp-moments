#include <stdio.h>
#include <iostream>

class Base
{
public:
    virtual void f1() {}
    virtual void f2() {}
    virtual void f3() {}
    virtual void f4() {}
    virtual void f5() {}
    virtual int* hello() {
        len = 10;
        printf("Base::hello()\n");
        int* data = new int[len];
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
