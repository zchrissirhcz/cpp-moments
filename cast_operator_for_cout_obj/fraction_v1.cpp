/**
 * 定义分数类并打印，版本1
 * 通过重载全局的 operator << 函数实现
 */

#include <iostream>
#include <string>

/// forward declaration
class Fraction;

/// 声明重载的全局操作符函数 <<
std::ostream& operator << (std::ostream& os, const Fraction& frac);

class Fraction
{
public:
    Fraction(int num, int den=1):
        numerator(num), denominator(den) {}

    friend std::ostream& operator << (std::ostream& os, const Fraction& frac); /// 友元声明

private:
    int numerator;   /// 分子
    int denominator; /// 分母
};

/// 重载函数的实现
std::ostream& operator << (std::ostream& os, const Fraction& frac)
{
    os << std::to_string(frac.numerator) << "/" << std::to_string(frac.denominator);
    return os;
}

int main() {

    Fraction f(3, 5);
    std::cout << "f is: " << f << std::endl;

    return 0;
}