#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

Main::Main() : Block(MAIN), _epollFd(-1) {}

Main::~Main() {
  if (_epollFd != -1) {
    LOG_INFO << "Closing _epollFd";
    close(_epollFd);
  }
}

void Main::addChild(Block &block) { server = static_cast<Server &>(block); }

void Main::addListen(size_t port) { (void)port; }

void Main::run() {
  int event_count;

  _epollFd = epoll_create(1);
  if (_epollFd == ERROR)
    throw std::runtime_error("epoll_create() failed");
  _event.events = EPOLLIN;
  _event.data.fd = server._serverFd;
  if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, server._serverFd, &_event) == ERROR)
    throw std::runtime_error("epoll_ctl() failed");
  // Event-Loop
  while (true) {
    event_count = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
    if (event_count == ERROR)
      throw std::runtime_error("epoll_wait() failed");
    for (int i = 0; i < event_count; i++) {
      if (_events[i].data.fd == server._serverFd)
        handleNewConnections();
      else
        handleClientData(i);
    }
  }
  close(_epollFd);
  close(server._serverFd);
}

void Main::handleNewConnections() {
  struct sockaddr_in clientAddr;
  struct epoll_event clientEvent;
  socklen_t clientAddrLen = sizeof(clientAddr);
  int clientFd;

  while (true) {
    clientFd = accept(server._serverFd, (struct sockaddr *)&clientAddr,
                      &clientAddrLen);
    if (clientFd < 0)
      break;
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == ERROR) {
      LOG_ERROR << "fcntl() on client failed";
      close(clientFd);
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

void Main::handleClientData(int i) {
  int clientFd = _events[i].data.fd;
  char buffer[1024] = {0};
  int bytesRead = read(clientFd, buffer, sizeof(buffer));

  if (bytesRead <= 0) {
    LOG_INFO << "Zero bytes read... disconnecting client FD: " << clientFd;
    close(clientFd);
  } else {
    LOG_INFO << "Received client data on FD " << clientFd << ":\n" << buffer;
    std::string response =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
        "17\r\n\r\nHello from epoll!";
    write(clientFd, response.c_str(), response.length());
    LOG_INFO << "Disconnecting client FD: " << clientFd;
    close(clientFd);
  }
}
