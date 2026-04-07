#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"
#include "Block.hpp"

void testEndpoints(int N, std::string argument, std::string *endpoints, size_t len) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::ifstream file(argument.c_str());
    MainBlock main;
    try {
        parseDirectives(main, file, 0);
        if (len != main.server.locations.size()) {
            std::cout << N << ": " << failure << std::endl;
            return;
        }
        for (size_t i = 0; i < len; i++) {
            if (endpoints[i] != main.server.locations[i].endpoint) {
                std::cout << N << ": " << failure << std::endl;
                return;
            }
        }
        std::cout << N << ": " << success << std::endl;
    } catch (std::exception &e) {
        // std::cout << e.what() << std::endl;
        std::cout << N << ": " << failure << std::endl;
    }
}