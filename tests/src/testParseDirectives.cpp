#include <iostream>
#include <exception>
#include "tests.hpp"
#include "parseConfig.hpp"
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
        parseConfig(main, file);
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

int testEndpoints(int N, std::string argument, std::string *endpoints, size_t len, size_t server) {
    std::string failure = std::string("parseDirectives()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseDirectives()  -->  ") + GREEN + "SUCCESS" + RESET;
    std::ifstream file(argument.c_str());
    Config main;
    int numBraces = 0;
    bool hasServer = false;
    try {
        parseDirectives(main, file, 0, numBraces, hasServer);
        if (main._servers.empty()) {
            std::cout << N << ": " << failure << std::endl;
            return FAILURE;
        }
        if (main._servers[server].locations.size() != len) {
            std::cout << N << ": " << failure << std::endl;
            return FAILURE;
        }
        for (size_t j = 0; j < len; j++) {
            if (endpoints[j] != main._servers[server].locations[j].endpoint) {
                std::cout << N << ": " << failure << std::endl;
                return FAILURE;
            }
         }
        std::cout << N << ": " << success << std::endl;
    } catch (std::exception &e) {
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

        if (main._servers.empty()) {
            std::cout << N << ": " << failure << std::endl;
            return FAILURE;
        }

        bool foundPort = false;
        for (size_t i = 0; i < main._servers.size(); ++i) {
            for (size_t j = 0; j < main._servers[i]._port.size(); ++j) {
                if (main._servers[i]._port[j] == static_cast<int>(port)) {
                    foundPort = true;
                    break;
                }
            }
            if (foundPort) break;
        }
        if (foundPort) {
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
