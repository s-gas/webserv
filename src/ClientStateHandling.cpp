#include "Cgi.hpp"
#include "Client.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <sys/wait.h>

void Client::setupCgi() {
  try {
    Cgi cgiHandler(response, request, server->locations[locationIndex]);

    cgiHandler.execScript(cgiReadFd, cgiWriteFd);
    cgiPid = cgiHandler.childPid;
    if (cgiWriteFd != -1 && !request.body.empty()) {
      state = WRITING;
    } else {
      state = PROCESSING;
    }
    startTime = time(NULL);
    bytesSent = 0;
    cgiBytesWritten = 0;
  } catch (const std::exception &e) {
    LOG_ERROR << "CGI Setup failed: " << e.what();
    prepareErrorResponse(response.status);
    state = SENDING;
  }
}

void Client::handleAction(int triggeredFd) {
  if (state == READING) {
    readRequestChunk();
  } else if (state == WRITING && triggeredFd == cgiWriteFd) {
    writeCgiChunk();
  } else if (state == PROCESSING && triggeredFd == cgiReadFd) {
    readCgiChunk();
  } else if (state == SENDING) {
    sendResponseChunk();
  }
}

void Client::readRequestChunk() {
  lastActTime = time(NULL);
  char buffer[4096];
  ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);

  if (bytes <= 0) {
    state = DONE;
    return;
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
  LOG_REQUEST
  if (!isRequestValid()) {
    prepareErrorResponse(response.status);
    state = SENDING;
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

void Client::writeCgiChunk() {
  ssize_t remaining = request.body.size() - cgiBytesWritten;
  if (remaining <= 0) {
    state = PROCESSING;
    return;
  }

  ssize_t sent =
      write(cgiWriteFd, request.body.c_str() + cgiBytesWritten, remaining);

  if (sent > 0) {
    cgiBytesWritten += sent;
    if (cgiBytesWritten >= request.body.size()) {
      state = PROCESSING;
    }
    lastActTime = time(NULL);
  } else {
    prepareErrorResponse("500");
    state = SENDING;
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
    writeCgiHeader();
    responseRaw = response.header + response.body;
    LOG_RESPONSE
    state = SENDING;
  } else {
    // pipe error
    prepareErrorResponse("500");
    state = SENDING;
  }
}

void Client::sendResponseChunk() {
  lastActTime = time(NULL);
  ssize_t remaining = responseRaw.size() - bytesSent;
  ssize_t sent =
      send(fd, responseRaw.c_str() + bytesSent, remaining, MSG_NOSIGNAL);

  if (sent > 0) {
    bytesSent += sent;
    if (bytesSent >= responseRaw.size()) {
      state = DONE;
    }
  } else {
    state = DONE;
  }
}

bool Client::is(ClientState s) { return s == state; }
