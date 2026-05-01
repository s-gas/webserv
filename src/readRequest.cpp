#include <iostream>
#include <stdexcept>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include "Http.hpp"

#define BUF_SIZE 4

size_t getContentLength(std::string request) {
    size_t contentLength = 0;
    std::string line;
    std::string text;
    std::istringstream stream(request);
    while (std::getline(stream, line)) {
        if (line.find("Content-Length") != std::string::npos) {
            std::istringstream linestream(line);
            linestream >> text >> contentLength;
            break;
        }
    }
    return contentLength;
}

std::string readRequest(int fd) {
    std::string request;
    char buffer[BUF_SIZE];
    size_t headerLength = 0;
    std::string emptyLine = "\r\n\r\n";
    while (true) {
        int bytesRead = read(fd, buffer, BUF_SIZE);
        if (bytesRead == -1) throw std::runtime_error("read() failed");
        if (bytesRead == 0) break;
        request.append(buffer, bytesRead);
        size_t pos;
        if ((pos = request.find(emptyLine)) != std::string::npos) {
            headerLength = pos + emptyLine.size();
            break;
        }
    }
    size_t contentLength = getContentLength(request);
    size_t contentSegmentLength = request.size() - headerLength;
    if (contentLength > contentSegmentLength) {
        size_t leftover = contentLength - contentSegmentLength;
        char bodyBuffer[leftover];
        int bytesRead = read(fd, bodyBuffer, leftover);
        if (bytesRead == -1) throw std::runtime_error("read() failed");
        request.append(bodyBuffer, bytesRead);
    }
    std::cout << request << std::endl;
    return request;
}
