#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"
#include "Server.hpp"

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
    Main main;
    int numBraces = 0;
    try {
        parseDirectives(main, file, 0,numBraces);
        std::cout << N << ": " << pass << std::endl;
    } catch (std::exception &e) {
        // std::cout << e.what() << std::endl;
        std::cout << N << ": " << except << std::endl;
    }
}

void testEndpoints(int N, std::string argument, std::string *endpoints, size_t len) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::ifstream file(argument.c_str());
    Main main;
    int numBraces = 0;
    try {
        parseDirectives(main, file, 0, numBraces);
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

void testListenDirective(int N, std::string argument, size_t port) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::ifstream file(argument.c_str());
    Main main;
    int numBraces = 0;
    try {
        parseDirectives(main, file, 0, numBraces);
        if (main.server._port == static_cast<int>(port)) {
            std::cout << N << ": " << success << std::endl;
        } else {
            std::cout << N << ": " << failure << std::endl;
        }
    } catch (std::exception &e) {
        // std::cout << e.what() << std::endl;
        std::cout << N << ": " << failure << std::endl;
    }
}