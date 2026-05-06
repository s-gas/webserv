#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Http.hpp"
#include <map>

class Server;

class Client {

public:
    HttpRequest request;
    HttpResponse response;
    Server *server;
    int fd;

    Client();
    Client(Server &s, int clientFd);
    bool handleData();
};

#endif
