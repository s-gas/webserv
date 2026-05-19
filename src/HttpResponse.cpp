#include "Http.hpp"
#include <iostream>

HttpResponse::HttpResponse(): status("200"), version("HTTP/1.1"), server("webserv"), emptyLine("\r\n"), error(false){
    statuses["200"] = "OK";
    statuses["302"] = "FOUND";
    statuses["400"] = "BAD REQUEST";
    statuses["403"] = "FORBIDDEN";
    statuses["404"] = "NOT FOUND";
    statuses["405"] = "METHOD NOT ALLOWED";
    statuses["413"] = "CONTENT TOO LARGE";
    statuses["500"] = "INTERNAL SERVER ERROR";
}

void HttpResponse::print() {
    std::cout << std::endl << "RESPONSE:" << std::endl;
    std::cout << response << std::endl;
}
