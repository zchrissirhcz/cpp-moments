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

    operator std::string() const
    {
        std::string r = "(" + std::to_string(re) + ", " + std::to_string(im) + ")";
        return r;
    }

private:
    double re;
    double im;

    friend complex& __doapl(complex*, const complex&);
    friend complex& __doapi(complex*, const complex&);
    friend complex& __doaml(complex*, const complex&);
};

}