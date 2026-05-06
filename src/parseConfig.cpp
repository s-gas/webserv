#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include "Server.hpp"
#include "parseConfig.hpp"

bool isClosing(std::string line) {
    size_t pos = line.find("}");
    if (pos == line.npos) return false;
    for (size_t i = 0; i < line.length(); i++) {
        if (i == pos) continue;
        if (!isspace(line[i])) throw std::runtime_error("Invalid token");
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
    if (tokens.size() != 2) throw std::runtime_error("Invalid token");
    std::string value = removeSemicolon(tokens[1]);
    if (tokens[0] == "root") {
        block.root = value;
    } else if (tokens[0] == "index") {
        block.index = value;
    } else if (tokens[0] == "listen") {
        if (block.type != SERVER)
            throw std::runtime_error("Listen directive can be defined only in a server block");
        if (!isNumber(value)) throw std::runtime_error("Listen directive accepts only positive integers as parameter");
        int port = std::atoi(value.c_str());
        if (port <= 0 || port > 65536) {
            throw std::runtime_error("Invalid port number");
        }
        Server &server = static_cast<Server &>(block);
        server.addListen(port);
    }
}

void storeEndPoint(Location &location, std::string line) {
    std::vector<std::string> tokens = split(line);
    if (tokens.size() != 3 || tokens[0] != "location" || tokens[2] != "{") {
        throw std::runtime_error("Invalid token");
    }
    location.endpoint = tokens[1];
}

void parseDirectives(Block &block, std::ifstream &file, int level, int &numBraces, bool &hasServer) {
    if (level == 3) return;
    if (level != block.type) throw std::runtime_error("Invalid token");
    std::string line;
    while (std::getline(file, line)) {
        if (isClosing(line)) {
            numBraces--;
            if (numBraces < 0) throw std::runtime_error("Invalid token");
            return;
        } else if (isOpening(line)) {
            numBraces++;
            if (isBlock(line, "server")) {
                Server server;
                server.root = block.root;
                server.index = block.index;
                parseDirectives(server, file, level + 1, numBraces, hasServer);
                hasServer = true;
                Config &config = static_cast<Config &>(block);
                config.addChild(server);
            } else if (isBlock(line, "location")) {
                Location location;
                location.root = block.root;
                location.index = block.index;
                storeEndPoint(location, line);
                parseDirectives(location, file, level + 1, numBraces, hasServer);
                Server &server = static_cast<Server &>(block);
                server.addChild(location);
            } else {
                throw std::runtime_error("Block directive is missing");
            }
        } else if (isNotEmpty(line)) {
            parseDirective(line, block);
        }
    }
}

void parseConfig(Block &block, std::ifstream &file) {
    int numBraces = 0;
    bool hasServer = false;
    parseDirectives(block, file, 0, numBraces, hasServer);
    if (numBraces != 0) {
        throw std::runtime_error("Invalid number of curly braces");
    } else if (hasServer == false) {
        throw std::runtime_error("At least 1 server must be defined");
    }
}
