#include "Client.hpp"
#include "Cgi.hpp"
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
      serveCGI();
  } else {
      if (request.method == "GET") serveFile();
      else if (request.method == "POST") uploadFile();
  }
  return true;
}

bool Client::isRequestValid() {
    if (request.version != response.version) {
        response.status = "400";
        response.error = true;
    } else if ((locationIndex = isEndpoint()) == -1) {
        response.status = "404";
        response.error = true;
    } else if (isMethodAllowed() == false) {
        response.status = "405";
        response.error = true;
    } else if (isSizeOkay() == false) {
        response.status = "413";
        response.error = true;
    }
    return !response.error;
}

bool Client::isMethodAllowed() {
    Location location = server->locations[locationIndex];
    return location.methods.find(request.method) != location.methods.end();
}

int Client::isEndpoint() {
    int fallback = -1;
    for (size_t i = 0; i < server->locations.size(); i++) {
        Location &location = server->locations[i];
        if (request.endpoint.find(location.endpoint) == 0 && location.endpoint != "/") {
            LOG_INFO << "i " << i;
            return i;
        }
        if (server->locations[i].endpoint == "/") {
            fallback = i;
        }
    }
    return fallback;
}

bool Client::isSizeOkay() {
    Location location = server->locations[locationIndex];
    if (location.maxBodySize == 0) return true;
    return (request.contentLength < location.maxBodySize);
}

void Client::serveFile() {
    if (response.error == true) {
        writeError();
    } else {
        generatePath();
        readFile();
    }
    writeHeader(".html");
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}

void Client::serveCGI() {
    response.body = Cgi(request, server->locations[locationIndex]).execute();
    writeHeader(".html");
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}

void Client::uploadFile() {
    generatePath();
    writeFile();
    writeHeader(".html");
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}

void Client::generatePath() {
    Location location = server->locations[locationIndex];
    path = location.root;
    std::cout << location.endpoint << std::endl;
    path += location.endpoint == "/" ? request.endpoint : location.endpoint;
    if (request.file == "") path += location.index;
    std::cout << location.index << std::endl;
}

void Client::readFile() {
    std::ifstream file(path.c_str());
    if (file) {
        response.status = "200";
        response.body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    } else {
        response.status = "404";
        response.error = true;
        writeError();
    }
}

void Client::writeFile() {
    std::cout << path << std::endl;
    std::ofstream file(path.c_str());
    if (!file.is_open()) {
        response.status = "500";
        response.error = true;
        writeError();
        return;
    }
    response.status = "201";
    file << request.body;
    file.close();
}

void Client::writeError() {
    std::string path;
    path = server->errorsRoot + "/" + response.status + ".html";
    std::ifstream file(path.c_str());
    if (file) {
        response.body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    }
}

void Client::writeHeader(std::string extension) {
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server;
    ss << "Content-Type: " << server->contentTypes[extension] << "\r\n";
    ss << "Content-Length: " << response.body.size() << "\r\n";
    ss << response.emptyLine;
    response.header = ss.str();
}

bool Client::isCgi() {
    if (locationIndex == -1) return false;
    Location &location = server->locations[locationIndex];
    LOG_INFO << "isCgi() = " << !location.cgi.empty();
    LOG_INFO << "location.endpoint " << location.endpoint;
    LOG_INFO << "location.root " << location.root;
    LOG_INFO << "locationIndex " << locationIndex;
    return !location.cgi.empty();
}
