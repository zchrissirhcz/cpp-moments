/**
 * 
 * 逗号初始化，第四个版本
 * 能够用来初始化元素，对超出、少于元素数量的情况，也有检查
 * Array 模板类，改为 Matrix 模板类，用于二维数据的表示（以及后续的矩阵计算）
 * CommaInitializer类、用于打印的 operator << 重载函数，都作为 Matrix 类的友元
 */

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
            os << mat.data[i];
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