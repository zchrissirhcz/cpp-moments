#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>
#include <iostream>
#include <string>
#include <functional>

int qtest_current_fail_cnt = 0; // number of failures in one test function
int qtest_total_cnt = 0;
int qtest_fail_cnt = 0; // number of failures in one test set

#define QTEST_EXPECT(x, y, cond) \
    if (!((x)cond(y))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        if (strcmp(#cond, "==") == 0) \
        { \
            printf("Expected equality of these values:\n"); \
            printf("  %s\n", #x); \
            qtest_evaluate_if_required(#x, x); \
            printf("  %s\n", #y); \
            qtest_evaluate_if_required(#y, y); \
        } \
        else \
        { \
            printf("Expected: (%s) %s (%s), actual: %s vs %s\n", #x, #cond, #y, std::to_string(x).c_str(), std::to_string(y).c_str()); \
        } \
        qtest_current_fail_cnt++; \
    }

#define EXPECT_EQ(x, y) QTEST_EXPECT(x, y, ==)
#define EXPECT_NE(x, y) QTEST_EXPECT(x, y, !=)
#define EXPECT_LT(x, y) QTEST_EXPECT(x, y, <)
#define EXPECT_LE(x, y) QTEST_EXPECT(x, y, <=)
#define EXPECT_GT(x, y) QTEST_EXPECT(x, y, >)
#define EXPECT_GE(x, y) QTEST_EXPECT(x, y, >=)

#define EXPECT_TRUE(x) \
    if (!((x))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Value of: %s\n", #x); \
        printf("  Actual: false\n"); \
        printf("Expected: true\n"); \
        qtest_current_fail_cnt++; \
    }

#define EXPECT_FALSE(x) \
    if (((x))) \
    { \
        printf("%s:%u: Failure\n", __FILE__, __LINE__); \
        printf("Value of: %s\n", #x); \
        printf("  Actual: true\n"); \
        printf("Expected: false\n"); \
        qtest_current_fail_cnt++; \
    }

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

class TestEntity
{
private:
    TestEntity() { };
    ~TestEntity() { };
public:
    TestEntity(const TestEntity& other) = delete;
    TestEntity operator=(const TestEntity& other) = delete;
    
    static TestEntity& get_instance()
    {
        static TestEntity entity;
        return entity;
    }
    int add(std::function<void()> f, const char* fname)
    {
        test_funcs.push_back(f);
        test_func_names.push_back(fname);
        test_states.push_back(true);
        return 0;
    }
    int run_all_test_functions()
    {
        //for (auto f : test_funcs)
        for (int i = 0; i < test_funcs.size(); i++)
        {
            auto f = test_funcs[i];
            std::string fname = test_func_names[i];
            qtest_current_fail_cnt = 0;
            printf("[ RUN      ] %s\n", fname.c_str());
            f();
            if (qtest_current_fail_cnt == 0)
            {
                printf("[       OK ] %s (0 ms)\n", fname.c_str());
            }
            else
            {
                printf("[  FAILED  ] %s (0 ms)\n", fname.c_str());
                qtest_fail_cnt++;
            }
            test_states[i] = (qtest_current_fail_cnt == 0);
            qtest_total_cnt++;
        }

        printf("[----------] %d tests from c (0 ms total)\n", qtest_total_cnt);
        printf("\n");
        printf("[----------] Global test environment tear-down\n");
        printf("[==========] %d tests from 1 test suite ran. (0 ms total)\n", qtest_total_cnt);

        int passed_test_count = qtest_total_cnt - qtest_fail_cnt;
        printf("[  PASSED  ] %d test", passed_test_count);
        if (passed_test_count != 1)
        {
            printf("s");
        }
        printf(".\n");

        if (qtest_fail_cnt)
        {
            printf("[  FAILED  ] %d test", qtest_fail_cnt);
            if (qtest_fail_cnt > 1)
            {
                printf("s");
            }
            printf(", listed below:\n");
            for (int i = 0; i < qtest_total_cnt; i++)
            {
                if (!test_states[i])
                {
                    printf("[  FAILED  ] %s\n", test_func_names[i].c_str());
                }
            }
        }

        if (qtest_fail_cnt > 0)
        {
            printf("\n %d FAILED TEST", qtest_fail_cnt);
            if (qtest_fail_cnt > 1)
            {
                printf("S");
            }
            printf("\n");
        }

        return 0;
    }
public:
    std::vector<std::function<void()>> test_funcs;
    std::vector<std::string> test_func_names;
    std::vector<bool> test_states;
};

#define TEST(set, name) \
    void qtest_##set##_##name(); \
    int qtest_mark_##set##_##name = TestEntity::get_instance().add(qtest_##set##_##name, #set "." #name); \
    void qtest_##set##_##name() \

TEST(c, 1)
{
    EXPECT_EQ(1, 1);
}
TEST(c, 2)
{
    EXPECT_EQ(2, 3);
}

TEST(d, 1)
{
    EXPECT_NE(2, 3);
}
TEST(d, 2)
{
    EXPECT_NE(2, 2);
}

TEST(e, 1)
{
    EXPECT_LE(2, 3);
}
TEST(e, 2)
{
    EXPECT_LE(2, 2);
}

TEST(f, 1)
{
    EXPECT_LT(2, 5);
}

void qtest_init()
{
    int test_count = TestEntity::get_instance().test_funcs.size();
    int test_suite_count = 1;

    printf("[==========] Running %d tests from %d test suite.\n", test_count, test_suite_count);
    printf("[----------] Global test environment set-up.\n");
    printf("[----------] %d tests from c\n", test_count);
}

typedef struct qtest_state_t
{
    const char* name;
    bool state;
} qtest_state_t;

int RUN_ALL_TESTS()
{
    TestEntity::get_instance().run_all_test_functions();
    return 0;
}

int main()
{
    qtest_init();
    return RUN_ALL_TESTS();
}