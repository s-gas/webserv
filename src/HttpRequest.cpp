#include "Http.hpp"
#include "readRequest.hpp"
#include <cctype>
#include <ctype.h>

HttpRequest::HttpRequest() : method(""), version(""), contentType(""), contentLength(0), body("") {}

HttpRequest::HttpRequest(std::string str) : rawString(str), method(""), version(""), contentType(""), contentLength(0), body("") {
    std::string line;
    std::istringstream stream(str);
    int i = 0;
    while (std::getline(stream, line)) {
        if (i == 0) {
            std::vector<std::string> words = parseContent(line);
            method = words[0];
            endpoint = normalize(words[1]);
            setDirectoryAndFile();
            version = words[2];
        } else if (isHeaderField("Content-Type: ", line)) {
            std::vector<std::string> words = parseContent(line);
            contentType = words[1];
        } else if (isHeaderField("Content-Length: ", line)) {
            std::vector<std::string> words = parseContent(line);
            std::istringstream(words[1]) >> contentLength;
        } else if (line.find("\r") == 0) {
            body = parseBody(stream);
        }
        i++;
    }
}

void HttpRequest::setDirectoryAndFile() {
    if (endpoint.size() == 0) return;
    if (endpoint[endpoint.size() - 1] == '/') {
        directory = endpoint;
        file = "";
        return;
    }
    size_t lastSlash = endpoint.rfind('/');
    directory = endpoint.substr(0, lastSlash + 1);
    file = endpoint.substr(lastSlash + 1);
}

void HttpRequest::print() {
    std::cout << std::endl << "REQUEST:" << std::endl;
    std::cout << rawString << std::endl;
}

std::vector<std::string> parseContent(std::string &line) {
    std::istringstream linestream(line);
    std::vector<std::string> words(3);
    linestream >> words[0] >> words[1] >> words[2];
    return words;
}

std::string normalize(std::string str) {
    if (str.find(".") != std::string::npos) return str;
    return str.size() == 0 || str[str.size() - 1] == '/' ? str : str + '/';
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
