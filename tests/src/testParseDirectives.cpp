#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parse.hpp"
#include "Server.hpp"

int testParse(int N, std::string argument, std::string string) {
    std::string failure = std::string("parse()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parse()  -->  ") + GREEN + "SUCCESS" + RESET;
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
    Config main;
    try {
        parse(main, file);
        std::cout << N << ": " << pass << std::endl;
        return (string == "throw") ? 1 : 0;
    } catch (std::exception &e) {
        std::cout << N << ": " << except << std::endl;
        return (string == "throw") ? 0 : 1;
    }
}

int testParseDirectives(int N, std::string argument, std::string string) {
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
    Config main;
    int numBraces = 0;
    bool hasServer = false;
    try {
        parseDirectives(main, file, 0, numBraces, hasServer);
        std::cout << N << ": " << pass << std::endl;
        return (string == "throw") ? 1 : 0;
    } catch (std::exception &e) {
        std::cout << N << ": " << except << std::endl;
        return (string == "throw") ? 0 : 1;
    }
}

int testEndpoints(int N, std::string argument, std::string *endpoints, size_t len) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::ifstream file(argument.c_str());
    Config main;
    int numBraces = 0;
    bool hasServer = false;
    try {
        parseDirectives(main, file, 0, numBraces, hasServer);
        if (len != main.server.locations.size()) {
            std::cout << N << ": " << failure << std::endl;
            return FAILURE;
        }
        for (size_t i = 0; i < len; i++) {
            if (endpoints[i] != main.server.locations[i].endpoint) {
                std::cout << N << ": " << failure << std::endl;
                return FAILURE;
            }
        }
        std::cout << N << ": " << success << std::endl;
    } catch (std::exception &e) {
        // std::cout << e.what() << std::endl;
        std::cout << N << ": " << failure << std::endl;
        return FAILURE;
    }
    return SUCCESS;
}

int testListenDirective(int N, std::string argument, size_t port) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::ifstream file(argument.c_str());
    Config main;
    int numBraces = 0;
    bool hasServer = false;
    try {
        parseDirectives(main, file, 0, numBraces, hasServer);
        if (main.server._port == static_cast<int>(port)) {
            std::cout << N << ": " << success << std::endl;
        } else {
            std::cout << N << ": " << failure << std::endl;
            return FAILURE;
        }
    } catch (std::exception &e) {
        // std::cout << e.what() << std::endl;
        std::cout << N << ": " << failure << std::endl;
        return FAILURE;
    }
    return SUCCESS;
}
