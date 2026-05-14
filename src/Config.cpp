#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "signal.hpp"
#include <cerrno>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 1024

Config::Config() : Block(MAIN), epollFd(-1) {}

Config::~Config() {
  for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
    if (it->second.cgiReadFd != -1) {
      close(it->second.cgiReadFd);
    }
    close(it->first);
    LOG_INFO << "Force closed remaining client FD: " << it->first;
  }

  clients.clear();
  cgiToClient.clear();

  if (epollFd != -1) {
    LOG_INFO << "Closing epollFd";
    close(epollFd);
  }
}

// Methods

void Config::addChild(Server &server) { this->servers.push_back(server); }

int Config::init() {
  Log::setLogFile("webserv.log");
  epollFd = epoll_create(1);
  if (epollFd == ERROR) {
    if (errno == EINTR)
      return ERROR;
    throw std::runtime_error("epoll_create() failed");
  }

  // Loop through all server blocks
  for (size_t i = 0; i < servers.size(); ++i) {
    servers[i].init();

    for (size_t j = 0; j < servers[i].serverFd.size(); ++j) {
      int fd = servers[i].serverFd[j];

      event.events = EPOLLIN;
      event.data.fd = fd;
      if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == ERROR) {
        if (errno == EINTR)
          return ERROR;
        throw std::runtime_error("epoll_ctl() failed");
      }
    }
  }
  return SUCCESS;
}

void Config::updateEpollEvent(int fd, uint32_t events) {
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev) == ERROR) {
    LOG_ERROR << "Failed to modify epoll event for FD: " << fd;
  }
}

// Event-Loop
void Config::run() {
  int event_count;

  while (SignalState::serverRunning) {
    // Timeout of 1000ms for periodic cleanup
    event_count = epoll_wait(epollFd, events, MAX_EVENTS, 1000);
    if (event_count == ERROR) {
      if (errno == EINTR) return;
      throw std::runtime_error("epoll_wait() failed");
    }

    checkTimeouts();

    for (int i = 0; i < event_count; ++i) {
      int fd = events[i].data.fd;
      uint32_t event_types = events[i].events;

      // Handling socket errors immediately
      if (event_types & EPOLLERR) {
        LOG_ERROR << "Epoll error on FD: " << fd;
        if (cgiToClient.count(fd)) {
          int clientFd = cgiToClient[fd];
          clients[clientFd].prepareErrorResponse("500");
          clients[clientFd].state = SENDING_RESPONSE;
          updateEpollEvent(clientFd, EPOLLOUT);

          epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
          cgiToClient.erase(fd);
          clients[clientFd].cgiReadFd = -1;
        } else if (clients.count(fd)) {
          removeClient(fd);
        }
        continue;
      } else if ((event_types & EPOLLHUP) && !cgiToClient.count(fd)) {
        LOG_INFO << "Client hangup on FD: " << fd;
        removeClient(fd);
        continue;
      }

      int serverIndex = isServerFd(fd);

      // A: New connection
      if (serverIndex != -1) {
        handleNewConnections(fd, serverIndex);

        // B: Activity on cgi pipe
      } else if (cgiToClient.count(fd)) {
        int clientFd = cgiToClient[fd];
        ClientState prevState = clients[clientFd].state;

        clients[clientFd].handleAction(fd);

        if (prevState != SENDING_RESPONSE &&  clients[clientFd].is(SENDING_RESPONSE)) {
          epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
          cgiToClient.erase(fd);
          clients[clientFd].cgiReadFd = -1;
          updateEpollEvent(clientFd, EPOLLOUT);
        }

        if (clients[clientFd].is(FINISHED)) {
          removeClient(clientFd);
        }

        // C: Activity on client
      } else if (clients.count(fd)) {
        ClientState prevState = clients[fd].state;
        clients[fd].handleAction(fd);

        if (prevState != PROCESSING_CGI && clients[fd].is(PROCESSING_CGI) && clients[fd].cgiReadFd != -1) {
          registerCgiPipe(clients[fd].cgiReadFd, fd);
        }

        if (prevState != SENDING_RESPONSE && clients[fd].is(SENDING_RESPONSE)) {
          updateEpollEvent(fd, EPOLLOUT);
        }

        if (clients[fd].is(FINISHED))
          removeClient(fd);
      }
    }
  }
}

int Config::isServerFd(int fd) {
  for (size_t i = 0; i < servers.size(); ++i) {
    for (size_t j = 0; j < servers[i].serverFd.size(); ++j) {
      if (fd == servers[i].serverFd[j]) {
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
    clients[clientFd] = Client(servers[serverIndex], clientFd);
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == ERROR) {
      LOG_ERROR << "fcntl() on client failed";
      close(clientFd);
      continue;
    }
    LOG_INFO << "New client connected on FD: " << clientFd;
    clientEvent.events = EPOLLIN;
    clientEvent.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &clientEvent) == ERROR) {
      LOG_ERROR << "epoll_ctl() on client failed";
      close(clientFd);
    }
  }
}

void Config::registerCgiPipe(int pipeFd, int clientFd) {
  cgiToClient[pipeFd] = clientFd;

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = pipeFd;

  if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipeFd, &ev) == ERROR) {
    LOG_ERROR << "Failed to add CGI pipe to epoll";
  }
}

void Config::checkTimeouts() {
  time_t now = time(NULL);

  std::map<int, Client>::iterator it = clients.begin();
  while (it != clients.end()) {
    Client &c = it->second;
    int currentFd = it->first;

    // check CGI Timeout (30 sec)
    if (c.is(PROCESSING_CGI) && (now - c.startTime > 30)) {
      LOG_ERROR << "CGI Timeout on client FD " << c.fd;
      if (c.cgiReadFd != -1) {
        epoll_ctl(epollFd, EPOLL_CTL_DEL, c.cgiReadFd, NULL);
        close(c.cgiReadFd);
        cgiToClient.erase(c.cgiReadFd);
        c.cgiReadFd = -1;
      }
      c.handleTimeout();
      updateEpollEvent(c.fd, EPOLLOUT);
      ++it;
    // check client idle timeout
    } else if (now - c.lastActTime > 60) {
      LOG_INFO << "Idle Client Timeout. Disconnectin FD " << c.fd;
      ++it;
      removeClient(currentFd);
    } else {
      ++it;
    }
  }
}

void Config::removeClient(int fd) {
  if (clients.count(fd)) {
    int cgiFd = clients[fd].cgiReadFd;
    if (cgiFd != -1) {
      epoll_ctl(epollFd, EPOLL_CTL_DEL, cgiFd, NULL);
      close(cgiFd);
      cgiToClient.erase(cgiFd);
      clients[fd].cgiReadFd = -1;
    }

    if (clients[fd].cgiPid > 0) {
      kill(clients[fd].cgiPid, SIGKILL);
      waitpid(clients[fd].cgiPid, NULL, WNOHANG);
    }

    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    clients.erase(fd);
    LOG_INFO << "Client FD " << fd << " removed.";
  } else {
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
  }
}
