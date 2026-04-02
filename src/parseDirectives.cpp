#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <cctype>
#include "Block.hpp"

bool isClosing(std::string line) {
    size_t pos = line.find("}");
    if (pos == line.npos) return false;
    for (size_t i = 0; i < line.length(); i++) {
        if (i == pos) continue;
        if (!isspace(line[i])) throw std::runtime_error("Error: invalid token");
    }
    return true;
}

bool isOpening(std::string line) {
    size_t pos = line.find("{");
    if (pos == line.npos) return false;
    for (size_t i = pos + 1; i < line.length(); i++) {
        if (!isspace(line[i])) throw std::runtime_error("Error: invalid token");
    }
    return true;
}

bool isBlock(std::string line, std::string keyword) {
    size_t pos = line.find(keyword);
    if (pos == line.npos) return false;
    for (size_t i = 0; i < pos; i++) {
        if (!isspace(line[i])) throw std::runtime_error("Error: invalid token");
    }
    size_t start = pos + keyword.length();
    size_t end = line.find("{");
    for (size_t i = start; i < end; i++) {
        if (!isspace(line[i])) throw std::runtime_error("Error: invalid token");
    }
    return true;
}

bool isNotEmpty(std::string line) {
    for (size_t i = 0; i < line.length(); i++) {
        if (!isspace(line[i])) return true;
    }
    return false;
}

void parseDirective(std::string &line, Block &block) {
    (void)line;
    (void)block;
}

void parseDirectives(Block &block, std::ifstream &file, int level) {
    if (level == 3) return;
    if (level != block.type) throw std::runtime_error("Error: invalid token");
    std::string line;
    while (std::getline(file, line)) {
        if (isClosing(line)) {
            return;
        } else if (isOpening(line)) {
            if (isBlock(line, "server")) {
                ServerBlock server;
                parseDirectives(server, file, level + 1);
                block.add(server);
            } else if (isBlock(line, "location")) {
                LocationBlock location;
                //add endpoint to location
                parseDirectives(location, file, level + 1);
                block.add(location);
            }
        } else if (isNotEmpty(line)) {
            parseDirective(line, block);
        }
    }
}