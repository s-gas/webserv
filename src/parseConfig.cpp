#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include "Server.hpp"
#include "parseConfig.hpp"

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
                inheritDirectives(server, block);
                parseDirectives(server, file, level + 1, numBraces, hasServer);
                hasServer = true;
                Config &config = static_cast<Config &>(block);
                config.addChild(server);
            } else if (isBlock(line, "location")) {
                Location location;
                inheritDirectives(location, block);
                storeEndPoint(location, line);
                parseDirectives(location, file, level + 1, numBraces, hasServer);
                Server &server = static_cast<Server &>(block);
                server.addChild(location);
            } else {
                throw std::runtime_error("Block directive is invalid");
            }
        } else if (isNotEmpty(line)) {
            parseDirective(line, block);
        }
    }
}

void parseDirective(std::string &line, Block &block) {
    std::vector<std::string> tokens = split(line);
    if (tokens.size() <= 1) throw std::runtime_error("Invalid token");
    tokens[tokens.size() - 1] = removeSemicolon(tokens[tokens.size() - 1]);
    if (tokens[0] == "root") {
        block.root = parseSingleValue(tokens);
    } else if (tokens[0] == "index") {
        block.index = parseSingleValue(tokens);
    } else if (tokens[0] == "listen") {
        if (block.type != SERVER)
            throw std::runtime_error("Listen directive can be defined only in a server block");
        int port = parsePort(tokens);
        Server &server = static_cast<Server &>(block);
        server.addListen(port);
    } else if (tokens[0] == "cgi_pass") {
        if (block.type != LOCATION)
            throw std::runtime_error("Cgi directive can be defined only in a location block");
        Location &location = static_cast<Location &>(block);
        if (tokens.size() > 3)
            throw std::runtime_error("Cgi directive can only contain two tokens, cgi_pass <.file> <interpreter>");
        location.cgi[tokens[1]] = tokens[2];
    } else if (tokens[0] == "methods") {
        block.methods = parseMultipleValues(tokens);
    } else if (tokens[0] == "client_max_body_size") {
        block.maxBodySize = parseSize(tokens);
    }
}

std::string parseSingleValue(std::vector<std::string> tokens) {
    if (tokens.size() != 2)
        throw std::runtime_error("Invalid number of values");
    return tokens[1];
}

int parsePort(std::vector<std::string> tokens) {
    if (tokens.size() != 2)
        throw std::runtime_error("Invalid number of values");
    if (!isNumber(tokens[1])) throw std::runtime_error("Listen directive accepts only positive integers as parameter");
    int port = std::atoi(tokens[1].c_str());
    if (port <= 0 || port > 65536) {
        throw std::runtime_error("Invalid port number");
    }
    return port;
}

int parseSize(std::vector<std::string> tokens) {
    if (tokens.size() != 2)
            throw std::runtime_error("Invalid number of values");
    if (!isNumber(tokens[1])) throw std::runtime_error("Max body size directive accepts only positive integers as parameter");
    int size = std::atoi(tokens[1].c_str());
    return size;
}

std::set<std::string> parseMultipleValues(std::vector<std::string> tokens) {
    std::set<std::string> methods;
    for (size_t i = 1; i < tokens.size(); i++) {
        methods.insert(tokens[i]);
    }
    return methods;
}

void inheritDirectives(Block &child, Block &parent) {
    child.root = parent.root;
    child.index = parent.index;
    child.methods = parent.methods;
}
