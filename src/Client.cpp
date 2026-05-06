#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"
#include "print.hpp"

Client::Client() : server(NULL) {}

Client::Client(Server &s, int clientFd) : server(&s), locationIndex(-1), fd(clientFd) {}

bool Client::handleData() {
  std::string requestString = readRequest(fd);
  request = HttpRequest(requestString);
  printRequest(request.rawString);
  if (request.method == "GET") {
      serveFile();
  }
  printResponse(response.response);
  write(fd, response.response.c_str(), response.response.size());
  // return true when this Client is done
  return true;
}


void Client::serveFile() {
    getResponseStatus();
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server;
    std::string fileName;
    if (response.status >= "400") {
        fileName = server->root + "/" + response.status + ".html";
    } else if (locationIndex != -1) {
        Location location = server->locations[locationIndex];
        fileName = location.root + "/" + location.index;
    } else {
        fileName = server->root + "/" + server->index;
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

void Client::getResponseStatus() {
    if (request.version != response.version) {
        response.status = "400";
    } else if (isMethodAllowed() == false) {
        response.status = "405";
    } else if ((locationIndex = isEndpoint()) != -1) {
        response.status = "200";
    } else {
        response.status = "404";
    }
}


bool Client::isMethodAllowed() {
    return server->allowedMethods.find(request.method) != server->allowedMethods.end();
}

int Client::isEndpoint() {
    for (size_t i = 0; i < server->locations.size(); i++) {
        if (request.endpoint == server->locations[i].endpoint) return i;
    }
    return -1;
}
