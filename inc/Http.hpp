#ifndef HTTP_HPP
# define HTTP_HPP

#include <cstddef>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

class HttpRequest {
public:
    std::string method;
    std::string endpoint;
    std::string version;
    std::string contentType;
    std::size_t contentLength;
    std::string body;

    HttpRequest();
    HttpRequest(std::string rawString);
};

class HttpResponse {
public:
    std::map<std::string, std::string> statuses;
    std::string status;
    std::string version;
    std::string server;
    std::string emptyLine;
    std::string fileName;
    std::string response;
    std::string body;

    HttpResponse();

    void generate(HttpRequest &request);
    void generateHtml(HttpRequest &request);
};

std::vector<std::string> parseContent(std::string &line);
std::string parseBody(std::istringstream &stream);


#endif
