#include <iostream>

class Box
{
public:
    operator bool() const {
        std::cout << "[operator bool()]";
        return true;
    }
    operator char* () {
        std::cout << "[operator char*()]";
        return nullptr;
    }
};

void func(bool b) {
    std::cout << "   hello\n";
}

int main() {
    Box box;
    func(box);

    return 0;
}