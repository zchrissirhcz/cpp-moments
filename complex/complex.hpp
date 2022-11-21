#pragma once

#include <string>

namespace plain {

class complex
{
public:
    complex(double r=0, double i=0)
        : re(r), im(i)
    {}
    complex& operator+=(const complex&);
    complex& operator-=(const complex&);
    complex& operator*=(const complex&);
    double real() const { return re; }
    double imag() const { return im; }

private:
    double re;
    double im;

    friend complex& __doapl(complex*, const complex&);
    friend complex& __doapi(complex*, const complex&);
    friend complex& __doaml(complex*, const complex&);
};

}