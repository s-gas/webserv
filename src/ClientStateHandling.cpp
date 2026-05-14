#include "Client.hpp"
#include "Cgi.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"
#include <sys/wait.h>

void Client::handleAction(int triggeredFd) {
  if (state == READING_REQUEST) {
    readRequestChunk();
  } else if (state == PROCESSING_CGI && triggeredFd == cgiReadFd) {
    readCgiChunk();
  } else if (state == SENDING_RESPONSE) {
    sendResponseChunk();
  }
}

void Client::readRequestChunk() {
  char buffer[4096];
  ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);

  if (bytes <= 0) {
    state = FINISHED;
    return ;
  }
  requestRaw.append(buffer, bytes);

  if (requestRaw.find("\r\n\r\n") != std::string::npos) {
    request = HttpRequest(requestRaw);
    processRequest();
  }
}

void Client::processRequest() {
  if (!isRequestValid()) {
    prepareErrorResponse();
    state = SENDING_RESPONSE;
  } else if (isCgi()) {
    setupCgi();
  } else {
    prepareFileResponse();
    state = SENDING_RESPONSE;
  }
}

void Client::readCgiChunk() {
  char buffer[4096];
  ssize_t bytes = read(cgiReadFd, buffer, sizeof(buffer);

  if (bytes > 0) {
    response.body.append(buffer, bytes);
    startTime = time(NULL);
  } else {
    close(cgiReadFd);
    cgiReadFd = -1;
    waitpid(cgiPid, NULL, WNOHANG);

    writeCgiHeader();
    responseRaw = response.header + response.body;
    state = SENDING_RESPONSE;
  }
}

void Client::sendResponseChunk() {
  ssize_t remaining = responseRaw.size() - bytesSent;
  ssize_t sent = send(fd, responseRaw.c_str() + bytesSent, remaining, 0);

  if (sent >= 0) {
    bytesSent += sent;
    if (bytesSent >= responseRaw.size()) {
      state = FINISHED;
    }
  } else {
    state = FINISHED;
  }
}
