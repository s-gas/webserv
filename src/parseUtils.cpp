#include <cctype>
#include <string>
#include <iostream>

bool isNotEmpty(std::string line) {
    for (size_t i = 0; i < line.length(); i++) {
        if (!isspace(line[i])) return true;
    }
    return false;
}

void checkOnlySpaces(std::string line, size_t start, size_t end) {
    if (start > end) throw std::runtime_error("Error: parsing failure");
    for (size_t i = start; i < end; i++) {
        if (!isspace(line[i])) throw std::runtime_error("Error: invalid token");
    }
}

void checkEndPoint(std::string line, size_t start, size_t end) {
    if (start > end) throw std::runtime_error("Error: parsing failure");
    bool found = false;
    for (size_t i = start; i < end; i++) {
        if (!isspace(line[i])) {
            if (line[i] != '/') throw std::runtime_error("Error: invalid location's endpoint");
            found = true;
            while (i < end && !isspace(line[i])) i++;
            while (i < end) {
                if (!isspace(line[i])) {
                    throw std::runtime_error("Error: invalid token");
                }
                i++;
            }
        }
    }
    if (!found) throw std::runtime_error("Error: location's endpoint not specified");
}

void checkBetweenDirectiveAndBrace(std::string line, size_t start, size_t end, std::string type) {
    if (start > end) throw std::runtime_error("Error: parsing failure");
    if (type == "server") {
        checkOnlySpaces(line, start, end);
    } else if (type == "location") {
        checkEndPoint(line, start, end);
    }
}