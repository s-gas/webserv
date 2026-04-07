#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <cctype>
#include "Block.hpp"
#include "parse.hpp"

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
    checkOnlySpaces(line, pos + 1, line.length());
    return true;
}


bool isBlock(std::string line, std::string keyword) {
    size_t pos = line.find(keyword);
    if (pos == line.npos) return false;
    checkOnlySpaces(line, 0, pos);
    size_t start = pos + keyword.length();
    size_t end = line.find("{");
    if (keyword == "server") {
        checkBetweenDirectiveAndBrace(line, start, end, "server");
    } else if (keyword == "location") {
        checkBetweenDirectiveAndBrace(line, start, end, "location");
    }
    return true;
}

void parseDirective(std::string &line, Block &block) {
    (void)line;
    (void)block;
}

void storeEndPoint(LocationBlock &location, std::string line) {
    std::string string = "location";
    size_t start = line.find(string) + string.length();
    size_t end = line.length();
    std::string endpoint = "";
    for (size_t i = start; i < end; i++) {
        while (i < end && isspace(line[i])) i++;
        while (i < end && !isspace(line[i])) {
            endpoint += line[i];
            i++;
        }
        break;
    }
    location.endpoint = endpoint;
    std::cout << "endpoint: " << location.endpoint << std::endl;
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
                storeEndPoint(location, line);
                parseDirectives(location, file, level + 1);
                block.add(location);
            } else {
                throw std::runtime_error("Error: invalid token");
            }
        } else if (isNotEmpty(line)) {
            parseDirective(line, block);
        }
    }
}