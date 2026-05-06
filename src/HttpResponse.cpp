#include "Http.hpp"
#include <fstream>
#include <sstream>

HttpResponse::HttpResponse(): version("HTTP/1.1"), server("webserv\r\n"), emptyLine("\r\n") {
    statuses["200"] = "OK";
    statuses["400"] = "BAD REQUEST";
    statuses["404"] = "NOT FOUND";
    statuses["405"] = "METHOD NOT ALLOWED";
}

void HttpResponse::generate(HttpRequest &request) {
    if (request.version != version) {
        status = "400";
        generateHtml(request);
    } else if (request.endpoint == "/") {
        status = "200";
        generateHtml(request);
    } else {
        status = "404";
        generateHtml(request);
    }
}

void HttpResponse::generateHtml(HttpRequest &request) {
    std::ostringstream ss;
    ss << version << " " << status << " " << statuses[status] << "\r\n";
    ss << "Server: " << server;
    if (request.endpoint == "/") {
        fileName = std::string("www/") + "index" + ".html";
    } else {
        fileName = std::string("www/") + status + ".html";
    }
    std::ifstream file(fileName.c_str());
    if (file) {
        // .assign read an entire file into a string
        body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    } else {
      //fallback
      body = "<html><body><h1>" + status + " " + statuses[status] +
      "</h1></body><html>";
    }
    ss << "Content-Type: " << "text/html" << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << emptyLine;
    ss << body;
    response = ss.str();
}
