#include <ratio>
#include <string>

// make_unique for C++11
// https://stackoverflow.com/questions/26089319/is-there-a-standard-definition-for-cplusplus-in-c14
#if __cplusplus < 201402L
namespace std {

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}
#endif

class TestRegistry
{
public:
    static TestRegistry& get_instance()
    {
        static std::unique_ptr<TestRegistry> inner;
        if (inner == nullptr)
        {
            inner = std::make_unique<TestRegistry>();
        }
        return *inner;
    }
    int hello()
    {
        printf("hello world\n");
        return 0;
    }
};

int x = TestRegistry::get_instance().hello();
int y = TestRegistry::get_instance().hello();

int main()
{
    printf("hello, main()\n");

    return 0;
}