# 复习C++: complex 类的实现

在看候捷的 C++ 视频课程第三集第四集时节奏不太适应， 于是关掉视频手动从头实现 complex 类。

## 0x1 成员定义和构造函数的实现
```c++
class complex
{
public:
    complex(double r=0, double i=0)
        : re(r), im(i)
    {
    }

public:
    double re;
    double im;
};
```

使用
```c++
int main()
{
    complex c1(1, 2);
    complex c2;
    complex c3(3);
}
```

## 0x2 添加 `real()` 和 `imag()` 成员函数
需要把 `re` 和 `im` 两个成员从 public 修改为 private, 然后添加对应的 getter 函数。

c++ 类的成员函数， 如果不会修改数据成员， 那么就应该标记为 const:
```c++
class complex
{
public:
    ...
    double real() const { return re; } // 标记为了const， 这样的话以后定义 const complex c 对象时， c能正常用 real() 方法
};
```

使用
```c++
int main()
{
    const complex c1(1, 2);
    complex c2(3, 4);
    cout << c1.real() << endl;
    cout << c2.imag() << endl;
}
```

## 0x3 打印 complex 对象
简单正确的方法是， 通过重载全局的 `operator<<` 操作符函数实现:

```c++
std::ostream& operator<<(ostream& os, const complex& c)
{
    return os << "(" << c.real() << ", " << c.imag() << ")";
}
```

使用：
```c++
int main()
{
    complex c1(1, 2);
    cout << "c1: " << c1 << endl;
}
```

也曾尝试基于花哨的 conversion operator 实现 cout 打印对象， 但失败了。具体见 [conversion_operator](../conversion_operator/README.md).


## 0x4 支持两个复数的加法/减法等操作
预期的调用是：
```c++
int main()
{
    complex c1(1, 2);
    complex c2(3, 4);
    cout << c1 + c2 << endl;
}
```

有两种实现方式。

### 作为全局函数实现， 重载 `operator+` 操作符函数
```c++
complex operator+(const complex& c1, const complex& c2)
{
    return complex(c1.real() + c2.real(), c1.imag() + c2.imag());
}
```

好处： 不用修改 `complex` 类的定义， 可以只是在库的使用者一侧增加实现和调用， complex库的维护者不需要改变 API 和 ABI.

### 作为类的成员函数实现, 返回引用
```c++
class complex
{
public:
    ...
    complex& operator+(const complex& other)
    {
        re += other.real();
        im += other.imag();
        return *this;
    }
};
```

```c++
int main()
{
    complex c1(2, 1);
    complex c2(4, 0);

    cout << "c1 is: " << c1 << endl;
    cout << "c2 is: " << c2 << endl;

    cout << "c1 + c2: " << c1 + c2 << endl;
    cout << "c1 is: " << c1 << endl;
}
```

缺点： 会改变 API 和 ABI, 并且会改变 c1 自身（副作用）：
```
c1 is: (2, 1)
c2 is: (4, 0)
c1 + c2: (6, 1)
c1 is: (6, 1)
```

### 作为类的成员函数实现， 返回新的对象
```c++
class complex
{
public:
    ...
    complex operator+(const complex&);
    {
        return complex(re + other.real(), im + other.imag());
    }
}
```

输出：
```
c1 is: (2, 1)
c2 is: (4, 0)
c1 + c2: (6, 1)
c1 is: (2, 1)
```

好处： 不会改变 c1 本身。

### 其他考虑
考虑两个类 `A` 和 `B` 的对象之间的加法操作：
- 方式1: A+B, 可以是全局的 `operator+`, 或 `A::operator+(const B& b)`
- 方式2: B+A, 可以是全局的 `operator+`, 或 `A::operator+(const A& a)`
- 方式3: 还可以定义全局的 `operator+`, 然后在A或B中增加定义成员函数完成 A->B 或 B->A 的类型转换

以正数点类型 `Point` 和浮点数点类型 `PointF` 为例， 上述方式1和方式2需要同时实现（考虑表达式中的顺序）， 需要实现两个成员函数比较麻烦， 考虑用方式3实现：

```c++
#include <iostream>

using std::cout;
using std::endl;

class Point;

class PointF
{
public:
    PointF(float _x, float _y)
        : x(_x), y(_y)
    {}

public:
    float x;
    float y;
};


class Point
{
public:
    Point(int _x, int _y)
        : x(_x), y(_y)
    {}
    operator PointF() const
    {
        return PointF(x, y);
    }

public:
    int x;
    int y;
};


PointF operator+(const PointF& p1, const PointF& p2)
{
    return PointF(p1.x + p2.x, p1.y + p2.y);
}

std::ostream& operator<<(std::ostream& os, const Point& point)
{
    return os << "(" << point.x << ", " << point.y << ")";
}

std::ostream& operator<<(std::ostream& os, const PointF& point)
{
    return os << "(" << point.x << ", " << point.y << ")";
}

int main()
{
    Point pt(1, 2);
    cout << pt << endl;

    PointF ptf(3.1, 4.2);
    cout << ptf + pt << endl;
    cout << pt + ptf << endl;

    return 0;
}
```

输出如下， 符合预期：
```
(1, 2)
(4.1, 6.2)
(4.1, 6.2)
```

### 其他二元擦偶作的实现
依据数学公式进行实现即可
```c++

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

// 数字除法
complex operator/(const complex& c1, double s)
{
    return complex(c1.real() / s, c1.imag() / s);
}

// 共轭
complex conj(const complex& c1)
{
    return complex(c1.real(), -c1.imag());
}

// 范数
double norm(const complex& c1)
{
    return c1.real() * c1.real() + c1.imag() * c1.imag();
}

// 极坐标
complex polar(double rho, double theta)
{
    return complex(rho * cos(theta), rho * sin(theta));
}

// 判断相等
bool operator==(const complex& c1, const complex& c2)
{
    return c1.real() == c2.real() && c1.imag() == c2.imag();
}

// 判断不相等
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
```

## 0x5 复数的+=，-=等操作的实现
`+=` 操作等价于 `+` 之后再赋值：
```
c1 += c2
=>
c1 = c1 + c2
```
也就是说等号左边的对象 c1 发生了取值的改变。因此最好是在 `complex` 类中增加成员函数 `complex::operator+=(const complex& other)`. 如果是作为全局函数实现则还需要多一个赋值回来的步骤， 不划算。

这里的实现不是在 complex 类的body内实现的， 因此很多教程提到说要添加 `inline` 关键字（需要满足的大前提是： 放在 .hpp）：

```c++
// complex.hpp
inline complex& complex::operator+=(const complex& other)
{
    re += other.real();
    im += other.imag();
    return *this;
}
```

**如果打算放在 `.cpp` 中实现该函数， 则不能使用 `inline`, 否则链接器找不到符号**。正确写法是：
```c++
// complex.cpp
namespace plain {

complex& complex::operator+=(const complex& other)
{
    re += other.real();
    im += other.imag();
    return *this;
}

}
```


## 0x6 References
- [C++ 打印类对象: 基于cast operator](https://zhuanlan.zhihu.com/p/378435303)
- https://stackoverflow.com/questions/3044690/operator-stdstring-const