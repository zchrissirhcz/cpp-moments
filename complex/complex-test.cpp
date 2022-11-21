#include "complex.hpp"
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;
using namespace plain;


std::ostream& operator<<(std::ostream& os, const complex& x)
{
    return os << "(" << x.real() << ", " << x.imag() << ")";
}

complex operator+(const complex& c1, const complex& c2)
{
    return complex(c1.real() + c2.real(), c1.imag() + c2.imag());
}

complex operator-(const complex& c1, const complex& c2)
{
    return complex(c1.real() - c2.real(), c1.imag() - c2.imag());
}

complex operator*(const complex& c1, const complex& c2)
{
    // (a + bi) * (c + di) = (a*c - b*d, (a*d + b*c)i)
    double re = c1.real() * c2.real() - c1.imag() * c2.imag();
    double im = c1.real() * c2.imag() + c1.imag() * c2.real();
    return complex(re, im);
}

complex operator/(const complex& c1, double s)
{
    return complex(c1.real() / s, c1.imag() / s);
}

complex conj(const complex& c1)
{
    return complex(c1.real(), -c1.imag());
}

double norm(const complex& c1)
{
    return c1.real() * c1.real() + c1.imag() * c1.imag();
}

complex polar(double rho, double theta)
{
    return complex(rho * cos(theta), rho * sin(theta));
}

bool operator==(const complex& c1, const complex& c2)
{
    return (c1.real() == c2.real()) && (c1.imag() == c2.imag());
}

bool operator!=(const complex& c1, const complex& c2)
{
    return (c1.real()!=c2.real()) || (c1.imag() != c2.imag());
}

complex operator+(const complex& c1)
{
    return c1;
}

complex operator-(const complex& c1)
{
    return complex(-c1.real(), -c1.imag());
}

int main()
{
    complex c1(2, 1);
    complex c2(4, 0);

    cout << "c1 is: " << c1 << endl;
    cout << "c2 is: " << c2 << endl;

    cout << "c1 + c2: " << c1 + c2 << endl;
    cout << "c1 is: " << c1 << endl;

    cout << "c1 - c2: " << c1 - c2 << endl;
    cout << c1 * c2 << endl;
    cout << c1 / 2 << endl;

    cout << conj(c1) << endl;
    cout << norm(c1) << endl;
    cout << polar(10, 4) << endl;

    cout << (c1 += c2) << endl;

    cout << (c1 == c2) << endl;
    cout << (c1 != c2) << endl;
    cout << +c2 << endl;
    cout << -2 << endl;

    cout << (c2 - 2) << endl;
    cout << (5 + c2) << endl;

    return 0;
}
