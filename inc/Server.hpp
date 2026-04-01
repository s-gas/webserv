#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class Server {

public:
  Server(int port);
  ~Server(); // empty destructor

  // methods
  void init();
  void run();

private:
  int _port;
  int _serverFd;
  struct sockaddr_in _address;

  Server();                          // empty constructor
  Server(Server const &);            // copy constructor
  Server &operator=(Server const &); // copy assignment operator
};

#endif
