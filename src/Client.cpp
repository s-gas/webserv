#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"

Client::Client() : server(NULL) {}

Client::Client(Server &s, int clientFd) : server(&s), locationIndex(-1), fd(clientFd) {}

bool Client::handleData() {
  std::string requestString = readRequest(fd);
  request = HttpRequest(requestString);
  request.print();
  if (!isRequestValid()) {
      serveFile();
  } else if (isCgi()) {
      response.response = "Handle with CGI";
      write(fd, response.response.c_str(), response.response.size());
  } else {
      serveFile();
  }
  return true;
}

bool Client::isRequestValid() {
    if (request.version != response.version) {
        response.status = "400";
        response.error = true;
    } else if (isMethodAllowed() == false) {
        response.status = "405";
        response.error = true;
    } else if ((locationIndex = isEndpoint()) != -1) {
       if (!isCgi() && request.method != "GET") {
           response.status = "405";
           response.error = true;
       }
    } else {
        response.status = "404";
        response.error = true;
    }
    return !response.error;
}

bool Client::isMethodAllowed() {
    return server->allowedMethods.find(request.method) != server->allowedMethods.end();
}

int Client::isEndpoint() {
    int fallback = -1;
    for (size_t i = 0; i < server->locations.size(); i++) {
        if (request.endpoint == server->locations[i].endpoint) {
            return i;
        }
        if (server->locations[i].endpoint == "/") {
            fallback = i;
        }
    }
    return fallback;
}


void Client::serveFile() {
    writeBody();
    writeHeader();
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}

void Client::writeBody() {
    if (response.error == true) {
        writeError();
        return;
    }
    std::string fileName;
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
        writeError();
    }
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


void Client::writeHeader() {
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server;
    ss << "Content-Type: " << "text/html" << "\r\n";
    ss << "Content-Length: " << response.body.size() << "\r\n";
    ss << response.emptyLine;
    response.header = ss.str();
}

bool Client::isCgi() {
    if (locationIndex == -1) return false;
    Location location = server->locations[locationIndex];
    return location.cgi.size() != 0;
}
