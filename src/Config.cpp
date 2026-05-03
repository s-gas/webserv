#include "Http.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"
#include "signal.hpp"
#include <cerrno>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#define BUF_SIZE 1024

// OCF

Config::Config() : Block(MAIN), _epollFd(-1) {}

Config::~Config() {
  if (_epollFd != -1) {
    LOG_INFO << "Closing _epollFd";
    close(_epollFd);
  }
}

// Methods

void Config::addChild(Server &server) { this->_servers.push_back(server); }

int Config::init() {
  Log::setLogFile("webserv.log");
  _epollFd = epoll_create(1);
  if (_epollFd == ERROR) {
    if (errno == EINTR)
      return ERROR;
    throw std::runtime_error("epoll_create() failed");
  }

  // Loop through all server blocks
  for (size_t i = 0; i < _servers.size(); ++i) {
    _servers[i].init();

    for (size_t j = 0; j < _servers[i]._serverFd.size(); ++j) {
      int fd = _servers[i]._serverFd[j];

      _event.events = EPOLLIN;
      _event.data.fd = fd;
      if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &_event) == ERROR) {
        if (errno == EINTR)
          return ERROR;
        throw std::runtime_error("epoll_ctl() failed");
      }
    }
  }
  return SUCCESS;
}

// Event-Loop
void Config::run() {
  int event_count;

  while (SignalState::serverRunning) {
    event_count = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
    if (event_count == ERROR) {
      if (errno == EINTR)
        return;
      throw std::runtime_error("epoll_wait() failed");
    }

    for (int i = 0; i < event_count; i++) {
      int triggeredFd = _events[i].data.fd;
      if (isServerFd(triggeredFd))
        handleNewConnections(triggeredFd);
      else
        handleClientData(i);
    }
  }
}

int Config::isServerFd(int triggeredFd) {
  for (size_t i = 0; i < _servers.size(); ++i) {
    for (size_t j = 0; j < _servers[i]._serverFd.size(); ++j) {
      if (triggeredFd == _servers[i]._serverFd[j]) {
        return true;
      }
    }
  }
  return false;
}

void Config::handleNewConnections(int serverFd) {
  struct sockaddr_in clientAddr;
  struct epoll_event clientEvent;
  socklen_t clientAddrLen = sizeof(clientAddr);
  int clientFd;

  while (true) {
    clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientFd < 0)
      break;
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == ERROR) {
      LOG_ERROR << "fcntl() on client failed";
      close(clientFd);
      continue;
    }
    LOG_INFO << "New client connected on FD: " << clientFd;
    clientEvent.events = EPOLLIN;
    clientEvent.data.fd = clientFd;
    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &clientEvent) == ERROR) {
      LOG_ERROR << "epoll_ctl() on client failed";
      close(clientFd);
    }
  }
}

void Config::handleClientData(int i) {
  int clientFd = _events[i].data.fd;
  std::string requestString = readRequest(clientFd);
  HttpRequest request(requestString);
  const char *response =
      "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
      "17\r\n\r\nHello from epoll!";
  write(clientFd, response, 82);
  LOG_INFO << "Disconnecting client FD: " << clientFd;
  close(clientFd);
}
