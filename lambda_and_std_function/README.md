# lambda 转为 std::function
lambda 不能当作 std::function 类型传给函数。需要手动转换：
```c++
    std::function<double(double)> plus_one = [](double x) -> double {
        return x + 1;
    };
```

其中 `double(double)` 中， 第一个 double 表示返回值类型， 第二个 double 表示参数类型。
