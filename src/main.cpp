#include <iostream>
#include <exception>
#include "Server.hpp"

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: ./webserv [file.conf]" << std::endl;
        return 1;
    }
    std::string fileName = argc == 1 ? "default.conf" : argv[1];
    try {
        Server server(fileName);
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}