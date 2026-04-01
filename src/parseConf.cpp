#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include "Server.hpp"

#define EXT_LEN 5

void checkExtension(std::string fileName) {
    size_t length = fileName.size();
    std::string ext = ".conf";
    if (length < EXT_LEN || fileName.substr(length - EXT_LEN, EXT_LEN) != ext) {
        throw std::runtime_error("Error: Invalid extension");
    }
}

void checkForServerBlock(bool *isServerPresent, std::string line) {
    std::string serverStart = "server {";
    std::string serverEnd = "}";
    if (line.substr(0, serverStart.size()) == serverStart) {
        std::cout << "start is present" << std::endl;
        isServerPresent[0] = true;
    }
    if (line.substr(0, serverEnd.size()) == serverEnd) {
        std::cout << "end is present" << std::endl;
        isServerPresent[1] = true;
    }
}