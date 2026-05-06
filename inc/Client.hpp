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
    int locationIndex;
    int fd;

    Client();
    Client(Server &s, int clientFd);
    bool handleData();
    void serveFile();
    void getResponseStatus();
    bool isMethodAllowed();
    int isEndpoint();
};

#endif
