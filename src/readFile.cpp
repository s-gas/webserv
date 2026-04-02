#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

void readFile(std::ifstream &file, std::string &fileName) {
    file.open(fileName.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("ERROR: Failed to open file");
    }
}