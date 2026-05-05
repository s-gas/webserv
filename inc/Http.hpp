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
    std::string response;
    std::string version;

    HttpResponse();

    void generateResponse(std::string status);
};

std::vector<std::string> parseContent(std::string &line);
std::string parseBody(std::istringstream &stream);


#endif
