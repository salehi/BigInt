#include "BigInt.h"
#include <iostream>

BigInt fibonacci(int n) {
    BigInt a = 1;
    BigInt b = 1;
    BigInt placeholder;
    for (int index = 2; index < n; index++) {
        placeholder = a;
        a += b;
        b = placeholder;
    }
    return a;
}

int main() {
    BigInt a = "65536";
    BigInt b = "-65536";
    BigInt c = a + b;
    BigInt d = a - b;
    a *= b;
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << c << std::endl;
    std::cout << d << std::endl;
    b = fibonacci(10000);
    std::cout << b;
}
