#include <iostream>
#include <string>

// typedef basic_string<char>    string;

class X
{
public:
    // viable
    // operator double() {
    //     return 2.33;
    // }

    // not viable
    operator std::string() const {
        return std::string("str");
    }
};

std::ostream& operator<<(std::ostream& os, const std::string& s)
{
    return os << s;
}

int main()
{
    X x;
    std::cout << x;

    return 0;
}