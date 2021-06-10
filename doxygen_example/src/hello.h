#ifndef HELLO_H
#define HELLO_H

/**
  @defgroup basic_types Basic Types
  @{
      @defgroup array_type Array
      @defgroup matrix_type Matrix
      @defgroup tensor_type Tensor
      @defgroup network Nerual Network Data Types
      @{
          @defgroup layer Layer
          @defgroup network Network
      @}
  @}

  @defgroup math_algorithms Math and Algorithms
  @{
    @defgroup array_op Array Operations
    @defgroup matrix_op Matrix Operations
    @{
      @defgroup decomp Matrix Decomposition
      @{
          @defgroup decomp_lu LU Decomposition
          @defgroup decomp_svd SVD Decomposition
      @}
      @defgroup solve Solve Linear Equation
    @}
    @defgroup tensor_op Tensor Operations
  @}

  @defgroup deep_nn Deep Neural Networks

*/

typedef unsigned char uchar;

#include <memory>

//! @addtogroup basic_types
//! @{

/// Matrix class
///
/// represens the image struct
///
class Mat
{
public:
    Mat(): rows_(0), cols_(0), data_(NULL){}

public:
    int rows();
    int cols();
    unsigned char* data();

private:
    int rows_;
    int cols_;
    std::shared_ptr<uchar> data_;
};

class Point
{
public:
    Point(): x(0), y(0) {}
    Point(int _x, int _y): x(_x), y(_y) {}

public:
    int x, y;
};

class Size
{
public:
    Size(): height(0), width(0) {}
    Size(int _height, int _width): height(_height), width(_width) {}

public:
    int height;
    int width;
};

//! @} basic_types

#endif // HELLO_H