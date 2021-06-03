# C++ 矩阵类：逗号初始化

[TOC]
## 0x0 目的

使用过 Eigen / OpenCV 的矩阵类后，觉得逗号初始化很好用，想在自己写的矩阵库里有类似的实现。具体说，希望有如下使用体验：

```c++
Array arr(4);
arr << 1, 2, 3, 4;
std::cout << arr << std::endl;

Matrix<double> mat(2, 3);
mat << 1, 2, 3,
       4, 5, 6;
std::cout << mat << std::endl;
```

Array 可以看做是 Matrix 的特例。除了初始化 Matrix 的元素，Eigen::Matrix 初始化时，还会检查逗号初始化元素数量是否和 Matrix 对象本身元素数量相等：既不能多，也不能少。

## 0x1 原理

基本原理是通过重载 `<<` 和 `,` 操作符实现，考虑到边界检查，用了一些 trick。

当执行逗号初始化赋值时，先调用一次 `operator << ()` 操作符函数，然后调用若干次 `operator ,()` 操作符函数。通过重载这两个操作符，在里面记录和更新用于赋值的索引，可以实现正确的赋值、过量元素的检查。

看了 Eigen 的实现（在 `eigen\Eigen\src\Core\CommaInitializer.h` ），发现 `operator << ()` 返回的是一个 CommaInitializer 类型的对象，然后不断调用 CommaInitializer 对象的 `operator , ()` 函数，实现余下元素的赋值，并且在它的析构函数中，判断元素数量是否少于目标数量。

## 0x2 实现

不考虑拷贝构造函数、拷贝赋值操作符函数的前提下，从 Array 类写起，到最终的 Matrix 类，以一个 C++ 初学者的视角展示了实现过程。

### 版本1

- 逗号初始化，第一个版本
- 能够用来初始化元素，对超出元素数量的赋值有检查
- 但对于元素数量不足的赋值，缺乏检查

实现代码如下：
```c++

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
```

运行输出：
```
0, 0, 0, 0

1, 2, 3, 4

5, 6, 7, 8
```

可以看到，通过逗号表达式给含有4个元素的 Array 对象传入 4 个值，实现了赋值（初始化）操作，初步实现了目标。

### 版本2

- 逗号初始化，第二个版本
- 能够用来初始化元素，对超出、少于元素数量的情况，也有检查

```c++

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
```

运行输出：
```
0, 0, 0, 0

1, 2, 3, 4

0, 0, 0, 0

Error! 用于初始化的元素数量，多于Array元素数量
```
可以看到，刻意构造的输入5个元素给4元素的 Array，会触发报错，离目标又进了一步。

### 版本3

- 逗号初始化，第三个版本
- 能够用来初始化元素，对超出、少于元素数量的情况，也有检查
- Array类，增加了 template

```c++
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
```

运行输出：
```
0, 0, 0, 0

1, 2, 3, 4

0, 0, 0, 0

1.1, 2.2, 3.3, 4.4

0, 0, 0, 0

Error! 用于初始化的元素数量，多于Array元素数量
```
可以看到，增加模板后，无论是 `Array<int>`，还是 `Array<float>`，亦或是 `Array<double>`，都能够用逗号表达式进行赋值，以及赋值元素数量的检查。

### 版本4

- 逗号初始化，第四个版本
- 能够用来初始化元素，对超出、少于元素数量的情况，也有检查
- Array 模板类，改为 Matrix 模板类，用于二维数据的表示（以及后续的矩阵计算）
- CommaInitializer类、用于打印的 operator << 重载函数，都作为 Matrix 类的友元

```c++

#include <stdio.h>
#include <iostream>
#include <vector>


template<class T> class Matrix;

template<class T> class CommaInitializer;

template<class T>
std::ostream& operator << (std::ostream& os, const Matrix<T>& mat);

template<class T>
class Matrix
{
public:
    Matrix(int _rows, int _cols): rows(_rows), cols(_cols), len(_cols*_rows) {
        data = new T[rows*cols];
        for (int i=0; i<len; i++) {
            data[i] = 0;
        }
    }
    ~Matrix() {
        delete[] data;
    }
    Matrix(const Matrix& other) = delete;
    Matrix& operator = (const Matrix& other) = delete;
public:
    int rows, cols;
    T* data;
private:
    int len;
    friend class CommaInitializer<T>;
    friend std::ostream& operator << <T>(std::ostream& os, const Matrix<T>& mat);
};

template<class T>
class CommaInitializer
{
public:
    CommaInitializer()=delete;
    explicit CommaInitializer(Matrix<T>* _mat): mat(_mat), init_cnt(1) {}
    ~CommaInitializer() {
        if (init_cnt!=mat->len) {
            fprintf(stderr, "Error! 用于初始化的元素数量，少于 Matrix 元素数量\n");
            exit(1);
        }
    }
    CommaInitializer& operator , (double val)
    {
        if(init_cnt >= mat->len) {
            fprintf(stderr, "Error! 用于初始化的元素数量，多于 Matrix 元素数量\n");
            exit(1);
        }
        mat->data[init_cnt] = val;
        init_cnt++;
        return *this;
    }
private:
    Matrix<T>* const mat;
    int init_cnt;
};

template<class T>
CommaInitializer<T> operator << (Matrix<T>& mat, double val)
{
    if (mat.data==NULL) {
        fprintf(stderr, "mat data is NULL\n");
        exit(1);
    }
    mat.data[0] = val;
    return CommaInitializer<T>(&mat);
}

template<class T>
std::ostream& operator << (std::ostream& os, const Matrix<T>& mat)
{
    for (int i = 0; i < mat.rows; i++) {
        for (int j=0; j < mat.cols; j++) {
            if (j > 0) {
                os << ", ";
            }
            os << mat.data[i*mat.cols + j];
        }
        os << "\n";
    }
    return os;
}

void test_matrix()
{
    // 初始化数量，等于元素数量 int 类型
    {
        Matrix<int> mat(1, 4);
        std::cout << mat << std::endl;
        mat << 1, 2, 3, 4;
        std::cout << mat << std::endl;
    }
    
    // 初始化数量，等于元素数量 double 类型
    {
        Matrix<double> mat(1, 4);
        std::cout << mat << std::endl;
        mat << 1.1, 2.2, 3.3, 4.4;
        std::cout << mat << std::endl;
    }

    // 初始化数量，等于元素数量 double 类型。2个维度
    {
        Matrix<double> mat(2, 3);
        std::cout << mat << std::endl;
        mat << 1.1, 2.2, 3.3,
               4.4, 5.5, 6.6;
        std::cout << mat << std::endl;
    }

    // 初始化数量，小于元素数量
    if (0)
    {
        Matrix<double> mat(1, 4);
        std::cout << mat << std::endl;
        mat << 1, 2, 3;
        std::cout << mat << std::endl;
    }

    // 初始化数量，大于元素数量
    {
        Matrix<double> mat(1, 4);
        std::cout << mat << std::endl;
        mat << 1, 2, 3, 4, 5;
        std::cout << mat << std::endl;
    }
}

int main()
{
    test_matrix();

    return 0;
}
```

运行输出：
```
0, 0, 0, 0

1, 1, 1, 1

0, 0, 0, 0

1.1, 1.1, 1.1, 1.1

0, 0, 0
0, 0, 0

1.1, 1.1, 1.1
2.2, 2.2, 2.2

0, 0, 0, 0

Error! 用于初始化的元素数量，多于 Matrix 元素数量
```
可以看到，`Matrix` 模板类，能够使用逗号表达式进行初始化，正确的填充了2维数据；除了赋值元素数量的检查，还把都好表达式辅助类 `CommaInitializer` 以及全局重载的操作符函数 `operator << ` 作为友元声明，使得后续对于 Matrix 类的
私有成员也得以访问。

## 0x4 代码获取


## 0x5 References

https://www.zhihu.com/question/288288583
