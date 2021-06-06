/**
 * 定义分数类并打印，版本2
 * 通过重载 operator std::string() 函数实现
 */

#include <iostream>
#include <string>

class Fraction
{
public:
    Fraction(int num, int den=1):
        numerator(num), denominator(den) {}
    
    /// 重载 operator type 函数， type 为 double ， 注意不用写返回值类型
    operator double() const {
        std::cout << "[operator double()]";
        return numerator*1.0 / denominator;
    }

    /// 重载 operator type 函数， type 为 std::string ， 注意不用写返回值类型
    operator std::string() const {
        std::cout << "[operator std::string()]";
        return std::to_string(numerator) + "/" + std::to_string(denominator);
    }

private:
    int numerator;   /// 分子
    int denominator; /// 分母
};

int main()
{
    Fraction f(3, 5);
    double d = 4 + f;
    std::cout << "\n--- now let's print\n";
    std::cout << "f: " << f << std::endl;
    std::cout << "f: " << std::string(f) << std::endl;
    std::cout << d << std::endl;

    return 0;
}
