#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"


void testCheckDirectiveBlocks(int N, std::string argument, std::string string) {
    std::string failure = std::string("checkDirectiveBlocks()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("checkDirectiveBlocks()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::string pass;
    std::string except;
    if (string == "throw") {
        pass = failure;
        except = success;
    } else {
        pass = success;
        except = failure;
    }
    std::ifstream file(argument.c_str());
    try {
        checkDirectiveBlocks(file);
        std::cout << N << ": " << pass << std::endl;
    } catch (std::exception &e) {
        std::cout << N << ": " << except << std::endl;
    }
}