#include <iostream>

static int add(int a, int b) {
    return a + b;
}

int main() {
    int sum = add(1, 2);
    std::cout << sum << std::endl;

    return 0;
}
