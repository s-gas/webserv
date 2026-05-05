#include "Server.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

// public

Server::Server() : Block(SERVER) {
  /*
  this->_port.push_back(8080);

  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  this->_addr.push_back(addr);
  this->_serverFd.push_back(-1);
  */
}

Server::~Server() {
  for (size_t i = 0; i < _serverFd.size(); ++i) {
    if (_serverFd[i] != -1) {
      LOG_INFO << "Closing server socket " << i;
      close(_serverFd[i]);
      _serverFd[i];
    }
  }
}

// methods

void Server::addChild(Location &location) { locations.push_back(location); }

void Server::addListen(size_t port) {
  this->_port.push_back(port);

  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  this->_addr.push_back(addr);
  this->_serverFd.push_back(-1);
}

void Server::init() {
  if (_port.size() == 0) {
    this->addListen(8080);
  }
  // Loop through all ports defined for the server block
  for (size_t i = 0; i < _port.size(); ++i) {
    _serverFd[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd[i] == ERROR)
      throw std::runtime_error("Socket creation failed");
    if (fcntl(_serverFd[i], F_SETFL, O_NONBLOCK) == ERROR)
      throw std::runtime_error("fcntl() failed");
    int enable = 1;
    if (setsockopt(_serverFd[i], SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) ==
      ERROR)
      throw std::runtime_error("setsockopt() failed");

    _addr[i].sin_family = AF_INET;
    _addr[i].sin_port = htons(_port[i]);
    _addr[i].sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(_serverFd[i], (struct sockaddr *)&_addr[i], sizeof(_addr[i])) == ERROR)
      throw std::runtime_error("Bind failed");

    if (listen(_serverFd[i], 10) == ERROR)
      throw std::runtime_error("Listen failed");

    LOG_INFO << "Server is listening on port " << _port[i];
  }
}
