#include "Http.hpp"
#include <iostream>
#include <sstream>
#include <string>

void parseStartLine(HttpRequest &request, std::string &line) {
    std::istringstream stream(line);
    std::string slash;
    stream >> request.method >> slash >> request.version;
}

HttpRequest parseRequest(std::string &str) {
    HttpRequest request;
    std::string line;
    std::istringstream stream(str);
    int i = 0;
    while (std::getline(stream, line)) {
        if (i == 0) {
            parseStartLine(request, line);
        }
        i++;
    }
    return request;
}
