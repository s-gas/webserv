#include "Client.hpp"
#include "Cgi.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <sys/wait.h>

void Client:: setupCgi() {
  try {
    Cgi cgiHandler(response, request, server->locations[locationIndex]);

    cgiReadFd = cgiHandler.execScript();
    cgiPid = cgiHandler.childPid;
    state = PROCESSING_CGI;
    startTime = time(NULL);
    bytesSent = 0;
  } catch (const std::exception &e) {
    LOG_ERROR << "CGI Setup failed: " << e.what();
    prepareErrorResponse(response.status);
    state = SENDING_RESPONSE;
  }
}

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
  lastActTime = time(NULL);
  char buffer[4096];
  ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);

  if (bytes <= 0) {
    state = FINISHED;
    return ;
  }
  requestRaw.append(buffer, bytes);

  size_t headerEnd = requestRaw.find("\r\n\r\n");
  if (headerEnd != std::string::npos) {
    if (request.method.empty()) {
      request = HttpRequest(requestRaw);
    }

    size_t currentBodyLength = requestRaw.size() - (headerEnd + 4);
    if (currentBodyLength >= request.contentLength) {
      request.body = requestRaw.substr(headerEnd + 4, request.contentLength);
      processRequest();
    }
  }
}

void Client::processRequest() {
  if (!isRequestValid()) {
    prepareErrorResponse(response.status);
    state = SENDING_RESPONSE;
  } else if (isCgi()) {
    setupCgi();
  } else {
    generatePath();
    if (request.method == "GET") {
      prepareFileResponse();
    } else if (request.method == "POST") {
      prepareUploadResponse();
    } else if (request.method == "DELETE") {
      prepareDeleteResponse();
    }
  }
}

void Client::readCgiChunk() {
  char buffer[4096];
  ssize_t bytes = read(cgiReadFd, buffer, sizeof(buffer));

  if (bytes > 0) {
    // normal read
    response.body.append(buffer, bytes);
    startTime = time(NULL);
  } else if (bytes == 0) {
    // finished read
    waitpid(cgiPid, NULL, WNOHANG);
    writeCgiHeader();
    responseRaw = response.header + response.body;
    state = SENDING_RESPONSE;
  } else {
    // pipe error
    waitpid(cgiPid, NULL, WNOHANG);
    prepareErrorResponse("500");
    state = SENDING_RESPONSE;
  }
}

void Client::sendResponseChunk() {
  lastActTime = time(NULL);
  ssize_t remaining = responseRaw.size() - bytesSent;
  ssize_t sent = send(fd, responseRaw.c_str() + bytesSent, remaining, MSG_NOSIGNAL);

  if (sent >= 0) {
    bytesSent += sent;
    if (bytesSent >= responseRaw.size()) {
      state = FINISHED;
    }
  } else {
    state = FINISHED;
  }
}

bool Client::is(ClientState s) {
  return s == state;
}
