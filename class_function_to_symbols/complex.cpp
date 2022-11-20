#include <iostream>

using std::cout;
using std::endl;

class complex
{
public:
    complex (double r = 0, double i = 0)
        : re(r), im(i)
    {
    }

    complex& operator += (const complex& r);
    double real() const { return re; } // 第一个版本， 获取 re
    double imag() const { return im; }
    void real(double r) { re = r; }    // 第二个版本， 设置 re
private:
    double re, im;
};

int main()
{
    complex c1(2, 1);

    double r = c1.real();
    c1.real(3);

    return 0;
}