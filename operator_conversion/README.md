原本目的是实现多种用于 `std::cout << obj` 的方式。

没想到 conversion operator 有点坑：
- 碰到模板函数，模板实参推导不支持隐式类型转换
- 多个 conversion operator 的优先级（rank）
- conversion operator 的优先级，在碰到 template function 的时候又是怎样？
- conversion operator 前面用 explicit 修饰，结果又是怎样？

坑有点深，简单说就是老老实实定义一个专门用来打印的 `operator <<`
