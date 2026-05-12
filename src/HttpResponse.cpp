#include "Http.hpp"
#include <fstream>
#include <sstream>

HttpResponse::HttpResponse(): status("200"), version("HTTP/1.1"), server("webserv\r\n"), emptyLine("\r\n"), error(false){
    statuses["200"] = "OK";
    statuses["400"] = "BAD REQUEST";
    statuses["404"] = "NOT FOUND";
    statuses["405"] = "METHOD NOT ALLOWED";
    statuses["500"] = "INTERNAL SERVER ERROR";
}

void HttpResponse::print() {
    std::cout << "RESPONSE:" << std::endl;
    std::cout << response << std::endl;
}
