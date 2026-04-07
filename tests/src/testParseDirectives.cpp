#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"
#include "Block.hpp"

void testParseDirectives(int N, std::string argument, std::string string) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
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
    MainBlock main;
    try {
        parseDirectives(main, file, 0);
        std::cout << N << ": " << pass << std::endl;
    } catch (std::exception &e) {
        // std::cout << e.what() << std::endl;
        std::cout << N << ": " << except << std::endl;
    }
}