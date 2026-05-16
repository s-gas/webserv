#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "signal.hpp"

void Config::run() {
  int event_count;

  while (SignalState::serverRunning) {
    event_count = epoll_wait(epollFd, events, MAX_EVENTS, 1000);
    if (event_count == ERROR) {
      if (errno == EINTR)
        return;
      throw std::runtime_error("epoll_wait() failed");
    }

    checkTimeouts();
    for (int i = 0; i < event_count; ++i) {
      int fd = events[i].data.fd;
      uint32_t event_types = events[i].events;

      if (event_types & EPOLLERR) {
        handleEpollError(fd);
        continue;
      }

      int serverIndex = isServerFd(fd);
      if (serverIndex != -1) {
        handleNewConnections(fd, serverIndex);
      } else if (readPipes.count(fd)) {
        handleCgiRead(fd);
      } else if (writePipes.count(fd)) {
        handleCgiWrite(fd);
      } else if (clients.count(fd)) {
        handleClient(fd);
      }
    }
  }
}

void Config::handleEpollError(int fd) {
  LOG_ERROR << "Epoll error on FD: " << fd;
  if (readPipes.count(fd)) {
    int clientFd = readPipes[fd];
    clients[clientFd].prepareErrorResponse("500");
    clients[clientFd].state = SENDING;
    updateEpollEvent(clientFd, EPOLLOUT);
    removeCgiPipe(clients[clientFd].cgiReadFd, readPipes);
  } else if (writePipes.count(fd)) {
    int clientFd = writePipes[fd];
    removeCgiPipe(clients[clientFd].cgiWriteFd, writePipes);
    clients[clientFd].state = PROCESSING;
  } else if (clients.count(fd)) {
    removeClient(fd);
  }
}

void Config::handleCgiRead(int fd) {
  int clientFd = readPipes[fd];
  Client &c = clients[clientFd];
  ClientState prevState = c.state;

  c.handleAction(fd);
  if (prevState != SENDING && c.is(SENDING)) {
    removeCgiPipe(c.cgiReadFd, readPipes);
    updateEpollEvent(clientFd, EPOLLOUT);
  }
  if (c.is(DONE)) {
    removeClient(clientFd);
  }
}

void Config::handleCgiWrite(int fd) {
  int clientFd = writePipes[fd];
  Client &c = clients[clientFd];

  c.handleAction(fd);
  if (c.is(PROCESSING) || c.is(SENDING)) {
    removeCgiPipe(c.cgiWriteFd, writePipes);
  }
  if (c.is(DONE)) {
    removeClient(clientFd);
  }
}

void Config::handleClient(int fd) {
  Client &c = clients[fd];
  ClientState prevState = c.state;

  c.handleAction(fd);
  if (prevState != WRITING && prevState != PROCESSING &&
      (c.is(WRITING) || c.is(PROCESSING))) {
    registerCgiPipe(c);
  }
  if (prevState != SENDING && c.is(SENDING)) {
    updateEpollEvent(fd, EPOLLOUT);
  }
  if (c.is(DONE)) {
    removeClient(fd);
  }
}
