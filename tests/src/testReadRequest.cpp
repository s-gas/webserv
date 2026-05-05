#include "tests.hpp"
#include "readRequest.hpp"
#include <string>

int testGetContentLength(int N, std::string argument, size_t result) {
    std::string failure = std::string("getContentLength()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("getContentLength()  -->  ") + GREEN + "SUCCESS" + RESET;

    if (result == getContentLength(argument)) {
        std::cout << N << ": " << success << std::endl;
        return 0;
    } else {
        std::cout << N << ": " << failure << std::endl;
        return 1;
    }
}

int testIsHeaderField(int N, std::string headerFile, std::string line, bool result) {
    std::string failure = std::string("isHeaderField()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("isHeaderField()  -->  ") + GREEN + "SUCCESS" + RESET;

    if (isHeaderField(headerFile, line) == result) {
        std::cout << N << ": " << success << std::endl;
        return 0;
    } else {
        std::cout << N << ": " << failure << std::endl;
        return 1;
    }
}
