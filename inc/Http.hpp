#ifndef HTTP_HPP
# define HTTP_HPP

# include <iostream>

class HttpRequest {
public:
    std::string method;
    std::string version;
    std::string contentType;
    unsigned int contentLength;
    std::string body;
    HttpRequest();
};

HttpRequest parseRequest(std::string &str);

#endif
