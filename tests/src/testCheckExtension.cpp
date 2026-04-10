#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"

int testCheckExtension(int N, std::string argument, std::string string) {
    std::string failure = "checkExtension(\"" + argument + "\")  -->  " + RED + "FAIL" + RESET;
    std::string success = "checkExtension(\"" + argument + "\")  -->  " + GREEN + "SUCCESS" + RESET;
    std::string pass;
    std::string except;
    if (string == "throw") {
        pass = failure;
        except = success;
    } else {
        pass = success;
        except = failure;
    }
    try {
        checkExtension(argument);
        std::cout << N << ": " << pass << std::endl;
        return (string == "throw") ? 1 : 0;
    } catch (std::exception &e) {
        std::cout << N << ": " << except << std::endl;
        return (string == "throw") ? 0 : 1;
    }
}
