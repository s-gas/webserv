#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Http.hpp"

class Server;

class Client {
public:
    HttpRequest request;
    HttpResponse response;
    Server *server;
    std::string path;
    int locationIndex;
    int fd;

    Client();
    Client(Server &s, int clientFd);
    bool handleData();
    void serveFile();
    bool isRequestValid();
    bool isMethodAllowed();
    int isEndpoint();
    void writeHeader();
    void writeBody();
    void generatePath();
    void writeError();
    bool isCgi();
};

#endif
