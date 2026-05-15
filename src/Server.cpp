#include "Server.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

// public

Server::Server() : Block(SERVER){}

Server::~Server() {
  for (size_t i = 0; i < serverFd.size(); ++i) {
    if (serverFd[i] != -1) {
      LOG_INFO << "Closing server fd " << serverFd[i];
      close(serverFd[i]);
      serverFd[i];
    }
  }
}

// methods

void Server::addChild(Location &location) { locations.push_back(location); }

void Server::addListen(size_t port) {
  this->port.push_back(port);

  struct sockaddr_in tmpAddr;
  std::memset(&tmpAddr, 0, sizeof(tmpAddr));
  this->addr.push_back(tmpAddr);
  this->serverFd.push_back(-1);
}

void Server::init() {
  if (port.size() == 0) {
      throw std::runtime_error("No port specified");
  }
  // Loop through all ports defined for the server block
  for (size_t i = 0; i < port.size(); ++i) {
    serverFd[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd[i] == ERROR)
      throw std::runtime_error("Socket creation failed");
    if (fcntl(serverFd[i], F_SETFL, O_NONBLOCK) == ERROR)
      throw std::runtime_error("fcntl() failed");
    int enable = 1;
    if (setsockopt(serverFd[i], SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) ==
      ERROR)
      throw std::runtime_error("setsockopt() failed");

    addr[i].sin_family = AF_INET;
    addr[i].sin_port = htons(port[i]);
    addr[i].sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverFd[i], (struct sockaddr *)&addr[i], sizeof(addr[i])) == ERROR)
      throw std::runtime_error("Bind failed");

    if (listen(serverFd[i], SOMAXCONN) == ERROR)
      throw std::runtime_error("Listen failed");

    LOG_INFO << "Server is listening on port " << port[i];
  }
}
