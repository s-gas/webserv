#include "Server.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

// public

Server::Server()
: Block(SERVER), _port(8080) {}

Server::~Server() {
  if (_serverFd != -1) {
    LOG_INFO << "Closing server socket";
    close(_serverFd);
  }
}

// methods

void Server::addChild(Block &block) {
    locations.push_back(static_cast<Location&>(block));
}

void Server::addListen(size_t port) {
    this->_port = port;
}

void Server::init() {

  Log::setLogFile("webserv.log");

  _serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_serverFd == ERROR)
    throw std::runtime_error("Socket creation failed");

  _address.sin_family = AF_INET;
  _address.sin_port = htons(_port);
  _address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(_serverFd, (struct sockaddr *)&_address, sizeof(_address)) == ERROR) {
    close(_serverFd);
    _serverFd = -1;
    throw std::runtime_error("Bind failed");
  }

  if (listen(_serverFd, 10) == ERROR) {
    close(_serverFd);
    _serverFd = -1;
    throw std::runtime_error("Listen failed");
  }

  LOG_INFO << "Server is listening on port " << _port;
}

void Server::run() {
  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  char buffer[1024];

  while (true) {
    clientAddrLen = sizeof(clientAddr);
    int newSocket =
        accept(_serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (newSocket == ERROR) {
      LOG_ERROR << "Failed to accept connection.";
      continue;
    }
    LOG_INFO << "Connection accepted!";

    std::memset(buffer, 0, sizeof(buffer));
    int bytesRead = read(newSocket, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
      std::cout << buffer << std::endl;
    }
    std::string httpResponse =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
        "12\r\n\r\nHello World!";
    write(newSocket, httpResponse.c_str(), httpResponse.length());
    close(newSocket);
    LOG_INFO << "Connection closed!";
  }
}

// private ---------------------------------------------------------------------
// int _port;
// int _serverFd;
// struct sockaddr_in _address;
