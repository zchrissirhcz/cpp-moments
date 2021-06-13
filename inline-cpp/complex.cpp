#include <iostream>

class complex
{
public:
    complex (double r=0, double i=0)
        : re(r), im(i)
    {}
    complex& operator += (const complex& x);
    double real() const { return re; }
    double imag() const { return im; }
private:
    double re, im;

    friend complex& __doapl (complex*, const complex&);
};

inline double
image(const complex& x)
{
    return x.imag();
}

int main()
{
    complex c1(2.5, 1.5);
    complex c2(2, 6);

    std::cout << c1.real() << std::endl;

    return 0;
}
