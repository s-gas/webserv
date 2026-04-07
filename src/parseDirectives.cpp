#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
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
    start = end + 1;
    end = line.length();
    checkOnlySpaces(line, start, end);
    return true;
}

void parseDirective(std::string &line, Block &block) {
    std::vector<std::string> tokens = split(line);
    if (tokens.size() != 2) throw std::runtime_error("Error: invalid token");
    std::string value = removeSemicolon(tokens[1]);
    if (tokens[0] == "root") {
        block.root = value;
    } else if (tokens[0] == "listen") {
        if (block.type != SERVER)
            throw std::runtime_error("Error: listen directive can be defined only in a server block");
        if (!isNumber(value)) throw std::runtime_error("Error: listen directive accepts only positive integers as parameter");
        size_t port = std::atoi(value.c_str());
        block.addListen(port);   
    }
}

void storeEndPoint(LocationBlock &location, std::string line) {
    std::vector<std::string> tokens = split(line);
    if (tokens.size() != 3 || tokens[0] != "location" || tokens[2] != "{") {
        throw std::runtime_error("Error: invalid token");
    }
    location.endpoint = tokens[1];
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
                block.addChild(server);
            } else if (isBlock(line, "location")) {
                LocationBlock location;
                storeEndPoint(location, line);
                parseDirectives(location, file, level + 1);
                block.addChild(location);
            } else {
                throw std::runtime_error("Error: invalid token");
            }
        } else if (isNotEmpty(line)) {
            parseDirective(line, block);
        }
    }
}