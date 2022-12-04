```c++
#include <unordered_map>

enum PixelType
{
    RGB,
    RGBA,
    GRAY,
    NV21,
    NV12
};

// https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

// compiled with g++ 5.4, with -std=c++11, will see error
//std::unordered_map<PixelType, int, EnumClassHash> table;

// EnumClassHash as 3rd argument, ok
std::unordered_map<PixelType, int, EnumClassHash> table;
```