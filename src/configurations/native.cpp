#include <iostream>

#include "datatypes.h"


int maxVal(int a, int b) {
    if (a < b) {
        return b;
    } else {
        return a;
    }
}

int main() {
    std::cout << "running main" << std::endl;
    std::cout << "size is" << sizeof(ImuData) << std::endl;
}