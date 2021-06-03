/**
 * 
 * 逗号初始化，第三个版本
 * 能够用来初始化元素，对超出、少于元素数量的情况，也有检查
 * Array类，增加了 template
 */

#include <stdio.h>
#include <iostream>
#include <vector>

template<class T>
class Array
{
public:
    Array(int _len): len(_len) {
        data = new T[len];
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
    T* data;
};

template<class T>
class CommaInitializer
{
public:
    CommaInitializer()=delete;
    explicit CommaInitializer(Array<T>* _arr): arr(_arr), init_cnt(1) {}
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
    Array<T>* const arr;
    int init_cnt;
};

template<class T>
CommaInitializer<T> operator << (Array<T>& arr, double val)
{
    if (arr.data==NULL) {
        fprintf(stderr, "arr data is NULL\n");
        exit(1);
    }
    arr.data[0] = val;
    return CommaInitializer<T>(&arr);
}

template<class T>
std::ostream& operator << (std::ostream& os, const Array<T>& arr)
{
    for (int i = 0; i < arr.len; i++) {
        if (i > 0) {
            os << ", ";
        }
        os << arr.data[i];
    }
    os << "\n";
    return os;
}


void test_array()
{
    // 初始化数量，等于元素数量 int 类型
    {
        Array<int> arr(4);
        std::cout << arr << std::endl;
        arr << 1, 2, 3, 4;
        std::cout << arr << std::endl;
    }
    
    // 初始化数量，等于元素数量 double 类型
    {
        Array<double> arr(4);
        std::cout << arr << std::endl;
        arr << 1.1, 2.2, 3.3, 4.4;
        std::cout << arr << std::endl;
    }

    // 初始化数量，小于元素数量
    if (0)
    {
        Array<double> arr(4);
        std::cout << arr << std::endl;
        arr << 1, 2, 3;
        std::cout << arr << std::endl;
    }

    // 初始化数量，大于元素数量
    {
        Array<double> arr(4);
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