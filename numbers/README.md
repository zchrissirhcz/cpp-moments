# 数字

## `std::numeric_limits<double>::min()` 的坑
`std::numeric_limits<T>::max()` 返回类型 `T` 的最大值。 很容易让人觉得， `std::numeric_limits<T>::min()` 返回 `T` 类型的最小值。这是个误区。

`std::numeric_limist<int>::min()` 返回的是 -2147483648, 确实是32位整数表示下的最小数字。

但 `std::numeric_limits<double>::min()` 返回 `2.225074e-308`, 是一个大于0的数字。

`std::numeric_limits<double>::lowest()` 才是 double 类型的最小取值， 也就是 -1.797693e+308.

References:
- https://stackoverflow.com/questions/17070351/why-does-numeric-limitsmin-return-a-negative-value-for-int-but-positive-values
- https://en.cppreference.com/w/cpp/types/numeric_limits/lowest
- https://en.cppreference.com/w/cpp/types/numeric_limits/min