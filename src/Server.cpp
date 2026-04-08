#include "Server.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <fcntl.h>
#include <stdexcept>
#include <string>

// public

Server::Server() : Block(SERVER), _port(8080), _serverFd(-1) {
  std::memset(&_address, 0, sizeof(_address));
}

Server::~Server() {
  if (_serverFd != -1) {
    LOG_INFO << "Closing server socket";
    close(_serverFd);
  }
}

// methods

void Server::addChild(Block &block) {
  locations.push_back(static_cast<Location &>(block));
}

void Server::addListen(size_t port) { this->_port = port; }

void Server::init() {
  Log::setLogFile("webserv.log");

  _serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_serverFd == ERROR)
    throw std::runtime_error("Socket creation failed");
  if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == ERROR)
    throw std::runtime_error("fcntl() failed");
  int enable = 1;
  if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) ==
      ERROR)
    throw std::runtime_error("setsockopt() failed");

  _address.sin_family = AF_INET;
  _address.sin_port = htons(_port);
  _address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(_serverFd, (struct sockaddr *)&_address, sizeof(_address)) == ERROR)
    throw std::runtime_error("Bind failed");

  if (listen(_serverFd, 10) == ERROR)
    throw std::runtime_error("Listen failed");

  LOG_INFO << "Server is listening on port " << _port;
}

// private ---------------------------------------------------------------------
// int _port;
// int _serverFd;
// struct sockaddr_in _address;
