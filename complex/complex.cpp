#include "complex.hpp"

namespace plain {

complex& complex::operator+=(const complex& other)
{
    re += other.real();
    im += other.imag();
    return *this;
}

complex& complex::operator-=(const complex& other)
{
    re -= other.real();
    im -= other.imag();
    return *this;
}

complex& complex::operator*=(const complex& other)
{
    double new_re = re * other.real() - im * other.imag();
    double new_im = re * other.imag() + im * other.real();

    re = new_re;
    im = new_im;

    return *this;
}

}
