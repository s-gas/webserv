#include "Http.hpp"
#include <iostream>
#include <sstream>
#include <string>

void parseStartLine(HttpRequest &request, std::string &line) {
    std::istringstream stream(line);
    std::string slash;
    stream >> request.method >> slash >> request.version;
}

std::string parseContentType(std::string &line) {
    std::istringstream linestream(line);
    std::string text;
    std::string contentType;
    linestream >> text >> contentType;
    return contentType;
}

HttpRequest parseRequest(std::string &str) {
    HttpRequest request;
    std::string line;
    std::istringstream stream(str);
    int i = 0;
    while (std::getline(stream, line)) {
        if (i == 0) {
            parseStartLine(request, line);
        } else if (line.find("Content-Type: ") != std::string::npos) {
            request.contentType = parseContentType(line);
        }
        i++;
    }
    return request;
}
