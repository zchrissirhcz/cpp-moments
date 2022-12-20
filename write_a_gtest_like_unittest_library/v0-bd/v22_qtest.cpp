#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>
#include <iostream>
#include <string>
#include <functional>
#include <map>

int qtest_current_fail_cnt = 0; // number of failures in one test function

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
    std::string make_proper_str(size_t num, const std::string str, bool uppercase = false)
    {
        std::string res;
        if (num > 1)
        {
            if (uppercase)
                res = std::to_string(num) + " " + str + "S";
            else
                res = std::to_string(num) + " " + str + "s";
        }
        else
        {
            res = std::to_string(num) + " " + str;
        }
        return res;
    }

public:
    TestEntity(const TestEntity& other) = delete;
    TestEntity operator=(const TestEntity& other) = delete;
    
    static TestEntity& get_instance()
    {
        static TestEntity entity;
        return entity;
    }
    int add(std::string test_set_name, std::string test_name, std::function<void()> f, const char* fname)
    {
        TestItem item(f, fname);
        test_sets[test_set_name].test_items.emplace_back(item);
        total_test_func_count++;
        return 0;
    }
    int run_all_test_functions()
    {
        std::map<std::string, TestSet>::iterator it = test_sets.begin();
        for (; it != test_sets.end(); it++)
        {
            std::string test_set_name = it->first;
            TestSet& test_set = it->second;
            std::vector<TestItem>& test_items = test_set.test_items;
            std::string test_item_str = make_proper_str(test_items.size(), "test");
            printf("[----------] %s from %s\n", test_item_str.c_str(), it->first.c_str());
            for (int i = 0; i < test_items.size(); i++)
            {
                auto f = test_items[i].f;
                std::string fname = test_items[i].fname;
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
                test_items[i].success = (qtest_current_fail_cnt == 0);
            }
            printf("[----------] %s from %s (0 ms total)\n", test_item_str.c_str(), it->first.c_str());
            printf("\n");
        }

        printf("[----------] Global test environment tear-down\n");
        std::string tests_str = make_proper_str(total_test_func_count, "test");
        std::string suite_str = make_proper_str(test_sets.size(), "test suite");
        printf("[==========] %s from %s ran. (0 ms total)\n",
            tests_str.c_str(),
            suite_str.c_str()
        );

        int passed_test_count = total_test_func_count - qtest_fail_cnt;
        std::string how_many_test_str = make_proper_str(passed_test_count, "test");
        printf("[  PASSED  ] %s.\n", how_many_test_str.c_str());

        if (qtest_fail_cnt)
        {
            std::string failed_test_str = make_proper_str(qtest_fail_cnt, "test");
            printf("[  FAILED  ] %s, listed below:\n", failed_test_str.c_str());

            std::map<std::string, TestSet>::iterator it = test_sets.begin();
            for (; it != test_sets.end(); it++)
            {
                std::string test_set_name = it->first;
                TestSet test_set = it->second;
                std::vector<TestItem> test_items = test_set.test_items;
                for (int i = 0; i < test_items.size(); i++)
                {
                    if (!test_items[i].success)
                    {
                        printf("[  FAILED  ] %s\n", test_items[i].fname.c_str());
                    }
                }
            }
        }

        if (qtest_fail_cnt > 0)
        {
            std::string failed_test_str = make_proper_str(qtest_fail_cnt, "FAILED TEST", true);
            printf("\n %s\n", failed_test_str.c_str());
        }

        return 0;
    }
public:
    struct TestItem
    {
        std::function<void()> f;
        std::string fname;
        bool success;

        TestItem(std::function<void()> _f, std::string _fname):
            f(_f), fname(_fname), success(true)
        {}
    };

    struct TestSet
    {
        std::vector<TestItem> test_items;
    };

    std::map<std::string, TestSet> test_sets;

public:
    int total_test_func_count = 0;

private:
    int qtest_fail_cnt = 0; // number of failures in one test set
};

#define TEST(set, name) \
    void qtest_##set##_##name(); \
    int qtest_mark_##set##_##name = TestEntity::get_instance().add(#set, #name, qtest_##set##_##name, #set "." #name); \
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
    int test_suite_count = TestEntity::get_instance().test_sets.size();
    int test_count = TestEntity::get_instance().total_test_func_count;

    std::string test_suite_str = TestEntity::get_instance().make_proper_str(test_suite_count, "test suite");
    std::string test_count_str = TestEntity::get_instance().make_proper_str(test_count, "test");
    printf("[==========] Running %s from %s.\n", test_count_str.c_str(), test_suite_str.c_str());
    printf("[----------] Global test environment set-up.\n");
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