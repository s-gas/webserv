#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"
#include "print.hpp"

Client::Client() : server(NULL) {}

Client::Client(Server &s, int clientFd) : server(&s), fd(clientFd) {}

bool Client::handleData() {
  std::string requestString = readRequest(fd);
  request = HttpRequest(requestString);
  printRequest(request.rawString);
  validateRequest();
  generateResponse();
  printResponse(response.response);
  write(fd, response.response.c_str(), response.response.size());
  // return true when this Client is done
  return true;
}

void Client::validateRequest() {
    if (request.version != response.version) {
        response.status = "400";
    } else if (isMethodAllowed() == false) {
        response.status = "405";
    } else if (request.endpoint == "/") {
        response.status = "200";
    } else {
        response.status = "404";
    }
}

void Client::generateResponse() {
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server;
    std::string fileName;
    if (request.endpoint == "/") {
        fileName = server->root + "index" + ".html";
    } else {
        fileName = server->root + response.status + ".html";
    }
    std::ifstream file(fileName.c_str());
    if (file) {
        response.body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    }
    ss << "Content-Type: " << "text/html" << "\r\n";
    ss << "Content-Length: " << response.body.size() << "\r\n";
    ss << response.emptyLine;
    ss << response.body;
    response.response = ss.str();
}

bool Client::isMethodAllowed() {
    return server->allowedMethods.find(request.method) != server->allowedMethods.end();
}
