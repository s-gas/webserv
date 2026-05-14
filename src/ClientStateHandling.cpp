#include "Client.hpp"
#include "Cgi.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <sys/wait.h>

void Client:: setupCgi() {
  try {
    Cgi cgiHandler(response, request, server->locations[locationIndex]);

    cgiReadFd = cgiHandler.startAsync();
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

bool Client::is(ClientState s) {
  return s == state;
}

void Client::prepareFileResponse() {
  generatePath();
  readFile();
  writeHeader(".html");

  responseRaw = response.header + response.body;
  state = SENDING_RESPONSE;
}

void Client::prepareUploadResponse() {
  writeFile();
  writeHeader(".html");
  responseRaw = response.header + response.body;
  state = SENDING_RESPONSE;
}

void Client::prepareDeleteResponse() {
  if (std::remove(path.c_str()) != 0) {
    response.status = "404";
    response.error = true;
    writeError();
  } else {
    response.status = "200";
    response.body = "<html><body><h1>File deleted successfully</h1></body></html>\n";
  }
  writeHeader(".html");
  responseRaw = response.header + response.body;
  state = SENDING_RESPONSE;
}

void Client::prepareErrorResponse(std::string code) {
  if (!code.empty()) response.status = code;
  writeError();
  writeHeader(".html");
  responseRaw = response.header + response.body;
}

void Client::handleTimeout() {
  if (cgiPid > 0) {
    kill(cgiPid, SIGKILL);
    waitpid(cgiPid, NULL, WNOHANG);
  }
  if (cgiReadFd != -1) {
    close(cgiReadFd);
    cgiReadFd = -1;
  }
  prepareErrorResponse("504");
  state = SENDING_RESPONSE;
}
