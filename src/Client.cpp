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
    getStatus();
    writeBody();
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server;
    ss << "Content-Type: " << "text/html" << "\r\n";
    ss << "Content-Length: " << response.body.size() << "\r\n";
    ss << response.emptyLine;
    ss << response.body;
    response.response = ss.str();
}

void Client::getStatus() {
    if (request.version != response.version) {
        response.status = "400";
        response.error = true;
    } else if (isMethodAllowed() == false) {
        response.status = "405";
        response.error = true;
    } else if ((locationIndex = isEndpoint()) != -1) {
        response.status = "200";
    } else {
        response.status = "404";
        response.error = true;
    }
}

bool Client::isMethodAllowed() {
    return server->allowedMethods.find(request.method) != server->allowedMethods.end();
}

int Client::isEndpoint() {
    int fallback = -1;
    for (size_t i = 0; i < server->locations.size(); i++) {
        if (request.endpoint == server->locations[i].endpoint) {
            std::cout << "endpoint found: " << request.endpoint << std::endl;
            return i;
        }
        if (server->locations[i].endpoint == "/") {
            fallback = i;
            std::cout << "fallback" << std::endl;
        }
    }
    return fallback;
}

void Client::writeBody() {
    std::string fileName;
    if (locationIndex != -1) {
        Location location = server->locations[locationIndex];
        if (location.endpoint == "/") {
            fileName = location.root + request.endpoint + location.index;
        } else {
            fileName = location.root + location.endpoint + location.index;
        }
        std::ifstream file(fileName.c_str());
        if (file) {
            response.body.assign((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
            file.close();
        } else {
            response.status = "404";
            response.error = true;
        }
    }
    if (response.error == true) writeError();
}

void Client::writeError() {
    std::string fileName;
    fileName = server->errorsRoot + "/" + response.status + ".html";
    std::ifstream file(fileName.c_str());
    if (file) {
        response.body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    }
}
