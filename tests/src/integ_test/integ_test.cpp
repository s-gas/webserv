#include "integ_test.hpp"
#include <iostream>

int main() {
    std::cout << "\n=== Testing CGI Integration ===" << std::endl;
    testCgiGet();
    testCgiPost();
    testCgiFileErrors();
}
