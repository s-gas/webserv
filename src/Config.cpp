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

Config::Config() : Block(MAIN), _epollFd(-1) {
    allowedMethods.insert("GET");
    allowedMethods.insert("POST");
    allowedMethods.insert("DELETE");
}

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
      int fd = _events[i].data.fd;
      int serverIndex = isServerFd(fd);
      if (serverIndex != -1)
        handleNewConnections(fd, serverIndex);
      else if (clients[fd].handleData()) {
        removeClient(fd);
      }
    }
  }
}

void Config::removeClient(int fd) {
    if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL)) {
        LOG_ERROR << "Failed to remove FD " << fd;
    }
    close(clients[fd].fd);
    clients.erase(fd);
    LOG_INFO << "Disconnecting client FD: " << fd;
}

int Config::isServerFd(int fd) {
  for (size_t i = 0; i < _servers.size(); ++i) {
    for (size_t j = 0; j < _servers[i]._serverFd.size(); ++j) {
      if (fd == _servers[i]._serverFd[j]) {
        return i;
      }
    }
  }
  return -1;
}

void Config::handleNewConnections(int serverFd, int serverIndex) {
  struct sockaddr_in clientAddr;
  struct epoll_event clientEvent;
  socklen_t clientAddrLen = sizeof(clientAddr);
  int clientFd;

  while (true) {
    clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientFd < 0)
      break;
    clients[clientFd] = Client(_servers[serverIndex], clientFd);
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
