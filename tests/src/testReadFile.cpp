#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"

void testReadFile(int N, std::string argument, std::string string) {
    std::string failure = std::string("readFile()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("readFile()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::string pass;
    std::string except;
    if (string == "throw") {
        pass = failure;
        except = success;
    } else {
        pass = success;
        except = failure;
    }
    std::ifstream file;
    try {
        readFile(file, argument);
        std::cout << N << ": " << pass << std::endl;
    } catch (std::exception &e) {
        std::cout << N << ": " << except << std::endl;
    }
}