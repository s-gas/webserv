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
  readPipes.clear();

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
  socklen_t clientAddrLen = sizeof(clientAddr); int clientFd;

  while (true) {
    clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientFd < 0)
      break;
    fcntl(clientFd, F_SETFD, FD_CLOEXEC);
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

void Config::registerCgiPipe(Client &c) {
  if (c.cgiReadFd != -1) {
    readPipes[c.cgiReadFd] = c.fd;
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = c.cgiReadFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, c.cgiReadFd, &ev);
  }

  if (c.cgiWriteFd != -1) {
    writePipes[c.cgiWriteFd] = c.fd;
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = c.cgiWriteFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, c.cgiWriteFd, &ev);
  }
}

void Config::checkTimeouts() {
  time_t now = time(NULL);

  std::map<int, Client>::iterator it = clients.begin();
  while (it != clients.end()) {
    Client &c = it->second;
    int currentFd = it->first;

    // check CGI Timeout (30 sec)
    if ((c.is(PROCESSING) || c.is(WRITING)) && (now - c.startTime > 30)) {
      LOG_ERROR << "CGI Timeout on client FD " << c.fd;
      cleanUpClientCgi(c);
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
    Client &c = clients[fd];

    cleanUpClientCgi(c);
    killCgiProcess(c);
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    clients.erase(fd);
    LOG_INFO << "Client FD " << fd << " cleanly removed.";
  } else {
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
  }
}

void Config::removeCgiPipe(int &pipeFd, std::map<int, int> &pipeMap) {
  if (pipeFd != -1) {
    epoll_ctl(epollFd, EPOLL_CTL_DEL, pipeFd, NULL);
    close(pipeFd);
    pipeMap.erase(pipeFd);
    pipeFd = -1;
  }
}

void Config::cleanUpClientCgi(Client &c) {
  removeCgiPipe(c.cgiWriteFd, writePipes);
  removeCgiPipe(c.cgiReadFd, readPipes);
}

void Config::killCgiProcess(Client &c) {
  if (c.cgiPid > 0) {
    kill(c.cgiPid, SIGKILL);
    c.cgiPid = -1;
  }
}
