/**
 * 
 * 逗号初始化，第二个版本
 * 能够用来初始化元素，对超出、少于元素数量的情况，也有检查
 */

#include <stdio.h>
#include <iostream>
#include <vector>

class Array
{
public:
    Array(int _len): len(_len) {
        data = new double[len];
        for (int i=0; i<len; i++) {
            data[i] = 0;
        }
    }
    ~Array() {
        delete[] data;
    }
    Array(const Array& arr) = delete;
    Array& operator = (const Array& arr) = delete;

public:
    int len;
    double* data;
};

class CommaInitializer
{
public:
    CommaInitializer()=delete;
    explicit CommaInitializer(Array* _arr): arr(_arr), init_cnt(1) {}
    ~CommaInitializer() {
        if (init_cnt!=arr->len) {
            fprintf(stderr, "Error! 用于初始化的元素数量，少于Array元素数量\n");
            exit(1);
        }
    }
    CommaInitializer& operator , (double val)
    {
        if(init_cnt >= arr->len) {
            fprintf(stderr, "Error! 用于初始化的元素数量，多于Array元素数量\n");
            exit(1);
        }
        arr->data[init_cnt] = val;
        init_cnt++;
        return *this;
    }
private:
    Array* const arr;
    int init_cnt;
};

CommaInitializer operator << (Array& arr, double val)
{
    if (arr.data==NULL) {
        fprintf(stderr, "arr data is NULL\n");
        exit(1);
    }
    arr.data[0] = val;
    return CommaInitializer(&arr);
}

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

void test_array()
{
    // 初始化数量，等于元素数量
    {
        Array arr(4);
        std::cout << arr << std::endl;
        arr << 1, 2, 3, 4;
        std::cout << arr << std::endl;
    }

    // 初始化数量，小于元素数量
    if (0)
    {
        Array arr(4);
        std::cout << arr << std::endl;
        arr << 1, 2, 3;
        std::cout << arr << std::endl;
    }

    // 初始化数量，大于元素数量
    {
        Array arr(4);
        std::cout << arr << std::endl;
        arr << 1, 2, 3, 4, 5;
        std::cout << arr << std::endl;
    }
}

int main()
{
    test_array();

    return 0;
}