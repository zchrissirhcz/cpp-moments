#include <iostream>
#include <string>

using namespace std;


// std::ostream& operator<<(std::ostream& os, const std::string& s) {
//     return os << s;
// }

class Fraction
{
public:
    Fraction(int num, int den=1):
        numerator(num), denominator(den) {}
    
    explicit operator double() const {
        std::cout << "[operator double()]";
        return numerator*1.0 / denominator;
    }

    explicit operator std::string() const {
        std::cout << "[operator std::string()]";
        return std::to_string(numerator) + "/" + std::to_string(denominator);
    }

private:
    int numerator;
    int denominator;

#ifdef OVERLOAD_STREAM_OP
    friend std::ostream& operator << (std::ostream& os, const Fraction& frac);
#endif

};

#ifdef OVERLOAD_STREAM_OP
std::ostream& operator << (std::ostream& os, const Fraction& frac)
{
    std::cout << "[operator <<]";
    os << std::to_string(frac.numerator) << "/" << std::to_string(frac.denominator);
    return os;
}
#endif

int main()
{
    Fraction f(3, 5);
    //double d = 4 + f;
    //std::cout << "\n--- now let's print\n";
    std::cout << "f: " << f << std::endl;
    //std::cout << "f: " << std::string(f) << std::endl;
    //std::cout << d << std::endl;

    return 0;
}
