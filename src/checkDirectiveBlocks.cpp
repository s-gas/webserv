#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>

void addBlock(std::stack<std::string> &blocks, std::string &line, size_t pos) {
    if (line.find("server") != line.npos && line.find("server") < pos) {
        if (blocks.top() != "main") throw std::runtime_error("Error: Invalid token");
        blocks.push("server");
    } else if (line.find("location") != line.npos && line.find("location") < pos) {
        if (blocks.top() != "server") throw std::runtime_error("Error: Invalid token");
        blocks.push("location");
    } else {
        throw std::runtime_error("Error: Invalid token");
    }
}


void checkDirectiveBlocks(std::ifstream &file) {
    std::stack<std::string> blocks;
    blocks.push("main");
    std::string line;
    while (getline(file, line)) {
        size_t pos = line.find("{");
        if (pos != line.npos) addBlock(blocks, line, pos);
        pos = line.find("}");
        if (pos != line.npos) {
            if (blocks.size() < 2) throw std::runtime_error("Error: Invalid token");
            blocks.pop();
        }
    }
    if (blocks.size() != 1) throw std::runtime_error("Error: Invalid token");
}