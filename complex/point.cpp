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








