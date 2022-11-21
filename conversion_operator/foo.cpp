#include <string>
#include <iostream>

// class Foo
// {
// public:
//     explicit operator std::string() const
//     {
//         return "I am a foo!";
//     }
// };

// std::ostream& operator<<(std::ostream& os, const std::string& s)
// {
//     static int count = 0;
//     count++;
//     std::cout << "operator<<(os, string&) " << count << std::endl;
//     return os << s;
// }

// std::ostream& operator<<(std::ostream& os, const Foo& s)
// {
//     // static int count = 0;
//     // count++;
//     // std::cout << "operator<<(os, Foo&) " << count << std::endl;
//     return os << s;
// }


namespace v1 {

class Foo
{
public:
    operator int()
    {
        return 233;
    }
};

}

namespace v2 {

class Foo
{
public:
    operator double()
    {
        return 233.456;
    }
};

}

namespace v3 {

class Foo
{
public:
    explicit operator double()
    {
        return 233.456;
    }
};

}

namespace v4 {

class Foo
{
public:
    explicit operator std::string() const&
    {
        return "I am a foo!";
    }
};

}


namespace v5 {

class Foo
{
public:
    operator std::string() const&
    {
        return "I am a foo!";
    }
};
std::ostream& operator<<(std::ostream& os, const std::string& s)
{
    return os << s;
}
}

namespace v6 {
class Foo
{
public:
    explicit operator std::string() const&
    {
        return "I am a foo!";
    }
};
};

namespace v7 {
class Foo
{
public:
    operator std::string() const&
    {
        return "I am a foo!";
    }
};
std::string operator<<(std::ostream& os, std::string& s)
{
    return s;
}
}

int main()
{
    v7::Foo foo;
    std::cout << foo << std::endl;
    return 0;
}


class Point
{
public:
    Point(int _x = 0, int _y = 0)
        : x(_x), y(_y)
    {}

    operator std::string() const&
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

public:
    int x;
    int y;
};

