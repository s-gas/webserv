#include <cctype>
#include <string>
#include <iostream>
#include <vector>

bool isNotEmpty(std::string line) {
    for (size_t i = 0; i < line.length(); i++) {
        if (!isspace(line[i])) return true;
    }
    return false;
}

void checkOnlySpaces(std::string line, size_t start, size_t end) {
    if (start > end) throw std::runtime_error("Parsing failure");
    for (size_t i = start; i < end; i++) {
        if (!isspace(line[i])) throw std::runtime_error("Invalid token");
    }
}

void checkEndPoint(std::string line, size_t start, size_t end) {
    if (start > end) throw std::runtime_error("Parsing failure");
    bool found = false;
    for (size_t i = start; i < end; i++) {
        if (!isspace(line[i])) {
            if (line[i] != '/') throw std::runtime_error("Invalid location's endpoint");
            found = true;
            while (i < end && !isspace(line[i])) i++;
            while (i < end) {
                if (!isspace(line[i])) {
                    throw std::runtime_error("Invalid token");
                }
                i++;
            }
        }
    }
    if (!found) throw std::runtime_error("Location's endpoint not specified");
}

void checkBetweenDirectiveAndBrace(std::string line, size_t start, size_t end, std::string type) {
    if (start > end) throw std::runtime_error("Parsing failure");
    if (type == "server") {
        checkOnlySpaces(line, start, end);
    } else if (type == "location") {
        checkEndPoint(line, start, end);
    }
}

std::vector<std::string> split(std::string line) {
    std::vector<std::string> v;
    size_t start = 0;
    size_t end = 0;
    while (end < line.length()) {
        if (isspace(line[end])) {
            if (end - start > 0) v.push_back(line.substr(start, end - start));
            start = end + 1;
        }
        end++;
    }
    if (end - start > 0) v.push_back(line.substr(start, end - start));
    return v;
}

bool isNumber(std::string string) {
    for (size_t i = 0; i < string.length(); i++) {
        if (!isdigit(string[i])) return false;
    }
    return true;
}

std::string removeSemicolon(std::string token) {
    size_t pos = token.find(";");
    if (pos == token.npos || pos != token.length() - 1) {
        throw std::runtime_error("Simple directive must end with semicolon (;)");
    }
    return token.substr(0, token.length() - 1);
}
