#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"

Client::Client() {
  init();
}

Client::Client(Server &s, int clientFd) {
  init();
  server = &s;
  fd = clientFd;
}

void Client::init() {
  server = NULL;
  locationIndex = -1;
  fd = -1;
  cgiWriteFd = -1;
  cgiBytesWritten = 0;
  cgiReadFd = -1;
  state = READING;
  cgiPid = -1;
  startTime = 0;
  lastActTime = time(NULL);
  bytesSent = 0;
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

void Client::generatePath() {
    Location location = server->locations[locationIndex];
    path = location.root + request.endpoint;
    if (request.file.empty() && location.autoIndex == false) path += location.index;
}


void Client::writeHeader(std::string extension) {
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server << "\r\n";
    ss << "Connection: close\r\n";
    ss << "Content-Type: " << server->contentTypes[extension] << "\r\n";
    ss << "Content-Length: " << response.body.size() << "\r\n";
    ss << response.emptyLine;
    response.header = ss.str();
}

void Client::writeCgiHeader() {
    std::ostringstream ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server << "\r\n";
    ss << "Connection: close\r\n";
    response.header = ss.str();
}

bool Client::isCgi() {
    if (locationIndex == -1) return false;
    Location &location = server->locations[locationIndex];
    return !location.cgi.empty();
}

