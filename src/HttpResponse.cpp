#include "Http.hpp"
#include <fstream>
#include <sstream>

HttpResponse::HttpResponse(): version("HTTP/1.1"), server("webserv\r\n"), emptyLine("\r\n") {
    statuses["200"] = "OK";
    statuses["400"] = "BAD REQUEST";
    statuses["404"] = "NOT FOUND";
    statuses["405"] = "METHOD NOT ALLOWED";
}
