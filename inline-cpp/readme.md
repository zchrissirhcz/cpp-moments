## inline 并非真的 inline

- inline 是对编译器的“建议”，复杂的函数，编译器也无能为力。
- 在 class body 内定义的成员函数，自动成为 inline 的，但这也只是对编译器的“建议”


```
inline 这个关键词建议直接忘记它有内联建议这个功能，因为现在编译器基本上无视 inline 的这个语义。
如果你真的确定要内联一个函数，MSVC 要用__forceinline , gcc 要用__attribute__((always_inline))。

inline 这个关键词现在的主要作用是允许一个符号在不同的编译单元有重复的定义，这样允许你在头文件里写出函数实现和 inline variable.
```

## 类的成员函数的 inline 修饰不能在 .cpp

只能在哪里出现呢？两个条件：
1. 在 .h 里
2. 并且，不是在 class body 内，而是在 class body 外

也就是说，如果类的成员函数的实现，不在 class body 内，则：
- 如果是在 .h 实现的，则可以用 inline 修饰
- 如果是在 .cpp 实现的，则不可以用 inline 修饰，因为会导致 符号找不到


