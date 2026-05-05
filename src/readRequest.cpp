#include <iostream>
#include <stdexcept>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include "Http.hpp"
#include "readRequest.hpp"

#define BUF_SIZE 1024

size_t getContentLength(std::string request) {
    size_t contentLength = 0;
    std::string line;
    std::string text;
    std::istringstream stream(request);
    while (std::getline(stream, line)) {
        if (isHeaderField("Content-Length: ", line)) {
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
        int leftover = contentLength - contentSegmentLength;
        char bodyBuffer[BUF_SIZE];
        while (leftover > 0) {
            int bytesRead = read(fd, bodyBuffer, std::min(leftover, BUF_SIZE));
            if (bytesRead == -1) throw std::runtime_error("read() failed");
            request.append(bodyBuffer, bytesRead);
            leftover -= BUF_SIZE;
        }
    }
    std::cout << request << std::endl;
    return request;
}

bool isHeaderField(std::string headerFile, std::string line) {
    for (size_t i = 0; i < line.length(); i++) {
        line[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(line[i])));
    }
    for (size_t i = 0; i < headerFile.length(); i++) {
        headerFile[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(headerFile[i])));
    }
    return (line.find(headerFile) != std::string::npos);
}
