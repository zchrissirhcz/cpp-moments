/**
 *
 * 逗号初始化，第一个版本
 * 能够用来初始化元素，对超出元素数量的赋值有检查
 * 但对于元素数量不足的赋值，缺乏检查
 *
 * 修复了 static 局部变量导致的 bug
 */

#include <stdio.h>
#include <iostream>
#include <vector>

class Array
{
public:
    Array(int _len) : len(_len) {
        data = new double[len];
        for (int i = 0; i < len; i++) {
            data[i] = 0;
        }
    }
    ~Array() {
        delete[] data;
    }
    Array(const Array& arr) = delete;
    Array& operator = (const Array& arr) = delete;

    friend std::ostream& operator << (std::ostream& os, const Array& arr);
    friend Array& operator , (Array& arr, double val);
    friend Array& operator << (Array& arr, double val);

public:
    int len;
    double* data;
private:
    void init_assign_value(double val)
    {
        if (init_value_cnt >= len) {
            fprintf(stderr, "too much elements for initialize\n");
            exit(1);
        }
        data[init_value_cnt] = val;
        init_value_cnt++;
    }
    int init_value_cnt;
};

std::ostream& operator << (std::ostream& os, const Array& arr)
{
    for (int i = 0; i < arr.len; i++) {
        if (i > 0) {
            os << ", ";
        }
        os << arr.data[i];
    }
    os << '\n';
    return os;
}

Array& operator , (Array& arr, double val)
{
    arr.init_assign_value(val);
    //return const_cast<Array&>(a);
    return arr;
}

Array& operator << (Array& arr, double val)
{
    arr.init_value_cnt = 0;
    arr.init_assign_value(val);
    return arr;
}

void test_array()
{
    Array arr(4);
    std::cout << arr << std::endl;
    arr << 1, 2, 3, 4;
    std::cout << arr << std::endl;

    Array arr2(4);
    arr2 << 5, 6, 7, 8;
    std::cout << arr2 << std::endl;
}

int main()
{
    test_array();

    return 0;
}
