#include "Http.hpp"

HttpRequest::HttpRequest() : method(""), version(""), contentType(""), contentLength(0), body("") {}

HttpRequest::HttpRequest(std::string str) : method(""), version(""), contentType(""), contentLength(0), body("") {
    std::string line;
    std::istringstream stream(str);
    int i = 0;
    while (std::getline(stream, line)) {
        if (i == 0) {
            std::vector<std::string> words = parseContent(line);
            method = words[0];
            version = words[2];
        } else if (line.find("Content-Type: ") != std::string::npos) {
            std::vector<std::string> words = parseContent(line);
            contentType = words[1];
        } else if (line.find("Content-Length: ") != std::string::npos) {
            std::vector<std::string> words = parseContent(line);
            std::istringstream(words[1]) >> contentLength;
        } else if (line.find("\r") == 0) {
            body = parseBody(stream);
        }
        i++;
    }
}

std::vector<std::string> parseContent(std::string &line) {
    std::istringstream linestream(line);
    std::vector<std::string> words(3);
    linestream >> words[0] >> words[1] >> words[2];
    return words;
}

std::string parseBody(std::istringstream &stream) {
    std::string line;
    std::string body = "";
    while (std::getline(stream, line)) {
        if (line.size() > 0 && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        body.append(line + "\n");
    }
    if (body.size() > 0) body.erase(body.size() - 1);
    return body;
}
