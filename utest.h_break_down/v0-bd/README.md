# QTEST： 从头实现一个不那么简陋的单元测试框架

## 0. 目的
根据 utest.h 的早期版本， 动手实现一个仿制品， 用于在 C/C++ 工程中以 header-only 的形式使用。

文本接下来使用 “client” 表示调用侧的代码， 使用 “dev” 表示测试框架的开发端的代码。

## 1. 断言 - 最简单的实现
打算实现的断言如下， 其中 `EXPECT_*` 系列不会导致程序终止， 仅输出 Failure 和文件、行号信息。`ASSERT_*` 系列则在输出报错信息的基础上， 让当前测试用例函数停止运行。

| EXPECT_* 系列    | ASSERT_* 系列   |
| ---------------- | --------------- |
| EXPECT_EQ(x, y)  | ASSERT_EQ(x, y) |
| EXPECT_NE(x, y)  | ASSERT_NE(x, y) |
| EXPECT_LT(x, y)  | ASSERT_LT(x, y) |
| EXPECT_LE(x, y)  | ASSERT_LE(x, y) |
| EXPECT_GT(x, y)  | ASSERT_GT(x, y) |
| EXPECT_GE(x, y)  | ASSERT_GE(x, y) |
| EXPECT_TRUE(x)   | ASSERT_TRUE(x)  |
| EXPECT_FALSE(x)  | ASSERT_FALSE(x) |

### 1.1 从最简单断言的输出开始复刻
v11_gtest.cpp 是最简单的断言的使用， 包括了两个测试用例， 每个测试用例仅使用 `EXPECT_EQ` 做检查， 并且检查结果是成功的。

运行结果如下：
![](v11_gtest_output.png)

在 v11_qtest.cpp 中复刻了 gtest 的测试用例和输出内容； 简单起见没有增加颜色, 并且大量的硬编码； `EXPECT_EQ` 定义为空的实现。输出如下：
![](v11_qtest_output.png)

### 1.2 正确的实现 `EXPECT_EQ()`
v12_gtest.cpp 在 v11_gtest.cpp 基础上， 把success的测试用例改为了fail:
```c++
TEST(c, 1)
{
    EXPECT_EQ(1, 1);
}

TEST(c, 2)
{
    EXPECT_EQ(2, 3); // 这里会fail
}
```
输出如下：
![](v12_gtest_output.png)

在 v12_qtest.cpp 中修改 `EXPECT_TRUE` 为如下：
```c++
#define EXPECT_EQ(x, y) \
    if ((x)!=(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Expected equality of these values:\n"); \
        printf("  %s\n  %s\n", #x, #y); \
    }
```

这样能够对于 failed 的case输出信息了。但是， gtest 还有统计信息， 包括同个 test set 内的统计， 以及所有 test set 的统计。当前我们的测试用例只有一个 set。通过对于每个测试用例函数维护一个 state， 分别记录测试用例的名称， 以及是否执行成功； 然后在执行测试用例的过程中， 如果 `EXPECT_EQ` 失败， 则标记当前用例的状态为 false。因此修改为如下定义：
```c++

bool qtest_success = true;
int qtest_total_cnt = 0;
int qtest_fail_cnt = 0;

#define EXPECT_EQ(x, y) \
    if ((x)!=(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Expected equality of these values:\n"); \
        printf("  %s\n  %s\n", #x, #y); \
        qtest_success = false; \
    } \
    else \
    { \
        qtest_success = true; \
    }
```

并在 `RUN_ALL_TESTS()` 中的每个测试用例执行前后， 维护它的状态：
```c++
int RUN_ALL_TESTS()
{
    qtest_state_t qtest_state[2]; // 暂且硬编码

    // testcase1
    qtest_state[qtest_total_cnt].name = "c.1";
    printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name);
    qtest_c_1();
    if (qtest_success)
    {
        printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name);
    }
    else
    {
        printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name);
        qtest_fail_cnt++;
    }
    qtest_state[qtest_total_cnt].state = qtest_success;
    qtest_total_cnt++;

    ...
}
```

输出如下：
![](v12_qtest_output.png)

### v1.3 支持 `EXPECT_NE()`
仿照 `EXPECT_EQ()` 即可。略。

### v1.4 支持更多的 `EXPECT_*` 断言
仿照 `EXPECT_EQ()` 可以实现 `EXPECT_LT()`, 但这样导致和 `EXPECT_NE()` 的代码基本一样， 这三个宏的代码考虑重构, 实现如下：
```c++
#define QTEST_EXPECT(x, y, cond) \
    if (!((x)cond(y))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        if (strcmp(#cond, "==") == 0) \
        { \
            printf("Expected equality of these values:\n"); \
            printf("  %s\n  %s\n", #x, #y); \
        } \
        else \
        { \
            printf("Expected: (%s) %s (%s), actual: %s vs %s\n", #x, #cond, #y, #x, #y); \
        } \
        qtest_success = false; \
    } \
    else \
    { \
        qtest_success = true; \
    }

#define EXPECT_EQ(x, y) QTEST_EXPECT(x, y, ==)
#define EXPECT_NE(x, y) QTEST_EXPECT(x, y, !=)
#define EXPECT_LT(x, y) QTEST_EXPECT(x, y, <)

// 顺带把其他几个 binary 断言也都实现了：
#define EXPECT_LE(x, y) QTEST_EXPECT(x, y, <=)
#define EXPECT_GT(x, y) QTEST_EXPECT(x, y, >)
#define EXPECT_GE(x, y) QTEST_EXPECT(x, y, >=)
```

同时， 在 `RUN_ALL_TESTS()` 中执行每个单元测试函数并统计状态信息时， 之前一直是手动处理每个case， 现在也考虑重构， 方法是定义一个宏来完成代码复用：

```c++
#define RUN_SINGLE_TEST(test_set, test_name) \
    { \
        qtest_state[qtest_total_cnt].name = #test_set "." #test_name; \
        printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name); \
        qtest_##test_set##_##test_name(); \
        if (qtest_success) \
        { \
            printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
        } \
        else \
        { \
            printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
            qtest_fail_cnt++; \
        } \
        qtest_state[qtest_total_cnt].state = qtest_success; \
        qtest_total_cnt++; \
    }

    // testcase1
    RUN_SINGLE_TEST(c, 1);
    RUN_SINGLE_TEST(c, 2);
    RUN_SINGLE_TEST(c, 3);
    RUN_SINGLE_TEST(c, 4);
    RUN_SINGLE_TEST(c, 5);
    RUN_SINGLE_TEST(c, 6);
    RUN_SINGLE_TEST(c, 7);
    RUN_SINGLE_TEST(c, 8);
    RUN_SINGLE_TEST(c, 9);
    RUN_SINGLE_TEST(c, 10);
    RUN_SINGLE_TEST(c, 11);
    RUN_SINGLE_TEST(c, 12);
```

对比 gtest 和 qtest 的输出， 在当前测试用例下， 只有代码行号的差别：
![](v14_gtest_qtest_compare.png)

### 1.5 EXPECT_TRUE, EXPECT_FALSE 的实现
比较 trivial, 实现如下, 运行效果和 gtest 一样
```c++
#define EXPECT_TRUE(x) \
    if (!((x))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Value of: %s\n", #x); \
        printf("  Actual: false\n"); \
        printf("Expected: true\n"); \
        qtest_success = false; \
    } \
    else \
    { \
        qtest_success = true; \
    }

#define EXPECT_FALSE(x) \
    if (((x))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Value of: %s\n", #x); \
        printf("  Actual: true\n"); \
        printf("Expected: false\n"); \
        qtest_success = false; \
    } \
    else \
    { \
        qtest_success = true; \
    }
```

### 1.6 实现 ASSERT_EQ()
`ASSERT_*` 相比于 `EXPECT_*` 系列， 区别在于一旦条件失败就结束当前函数， 而不是继续运行。 因此直接使用 return 可以（初步）达到目的， 实现如下：
```c++
#define ASSERT_EQ(x, y) \
    if ((x)!=(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Expected equality of these values:\n"); \
        printf("  %s\n", #x); \
        printf("  %s\n", #y); \
        qtest_current_fail_cnt++; \
        return; \
    }
```

`ASSERT_*` 系列断言的提前返回， 因此单元测试函数是否执行成功的逻辑考虑更新（顺带发现了之前的实现逻辑有bug）， 新的逻辑是 “统计单元测试函数中失败的断言数量， 大于0就表示失败， 等于0表示成功”。
```c++
int qtest_current_fail_cnt = 0; // number of failures in one test function
...
#define RUN_SINGLE_TEST(test_set, test_name) \
    { \
        qtest_state[qtest_total_cnt].name = #test_set "." #test_name; \
        qtest_current_fail_cnt = 0; \
        printf("[ RUN      ] %s\n", qtest_state[qtest_total_cnt].name); \
        qtest_##test_set##_##test_name(); \
        if (qtest_current_fail_cnt == 0) \
        { \
            printf("[       OK ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
        } \
        else \
        { \
            printf("[  FAILED  ] %s (0 ms)\n", qtest_state[qtest_total_cnt].name); \
            qtest_fail_cnt++; \
        } \
        qtest_state[qtest_total_cnt].state = (qtest_current_fail_cnt == 0); \
        qtest_total_cnt++; \
    }
```

考虑 `ASSERT_EQ(1+2, 2+2);` 这个 case：

对于传入 `ASSERT_EQ(x, y)` 的参数x或y不是单个变量而是表达式时， 例如 `1+2`,  gtest 会把表达式求值之后的结果打印出来（也就是3）。这个特性可以基于模板实现， 思路是把x或y这样的参数的字符串结果（编译期）传给函数， 并且把 x和y这样的表达式本身也传入， 传入的过程中（在运行期）会执行计算， 到了函数里头就是计算完的值了， 再交给 std::to_string() 得到字符串结果， 再和编译期确定的字符串比较， 如果不一样那就要额外打印。代码如下：

```c++
template<typename T>
void qtest_evaluate_if_required(const char* str, T value)
{
    if (strcmp(str, std::to_string(value).c_str()) != 0)
    {
        std::cout << "    Which is: " << value << std::endl;
    }
}

#define ASSERT_EQ(x, y) \
    if ((x)!=(y)) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Expected equality of these values:\n"); \
        printf("  %s\n", #x); \
        qtest_evaluate_if_required(#x, x); \
        printf("  %s\n", #y); \
        qtest_evaluate_if_required(#y, y); \
        qtest_current_fail_cnt++; \
        return; \
    }
```

实际上， 不仅仅是 `ASSERT_EQ()` 需要按需对传入的参数求值， 其他的断言宏也需要这样处理。 但是限于篇幅， 本小节先不处理其他宏了， 后续有空再统一处理。

### client 端代码
```c++

// 定义测试用例函数。
void testcase1()
{
    EXPECT_EQ(1, 1);
    EXPECT_NE(1, 2);
    EXPECT_LT(1, 2);
    EXPECT_LE(1, 2);
    EXPECT_GT(2, 1);
    EXPECT_GE(2, 1);
    EXPECT_TRUE(1);
    EXPECT_FALSE(0);
}

// 定义测试用例函数。
void testcase2()
{
    ASSERT_EQ(1, 1);
    //ASSERT_NE(1, 2);
    ASSERT_NE(1, 1); // 故意写一个失败的case，用于检查输出是否符合预期
    ASSERT_LT(1, 2);
    ASSERT_LE(1, 2);
    ASSERT_GT(2, 1);
    ASSERT_GE(2, 1);
    ASSERT_TRUE(1);
    ASSERT_FALSE(0);
}

int main()
{
    testcase1();
    testcase2();
    return 0;
}
```

### dev 端代码
