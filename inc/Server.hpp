#ifndef SERVER_HPP
# define SERVER_HPP

#include "Client.hpp"
#include "defines.hpp"
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <sys/epoll.h>

enum BlockType {
    MAIN,
    SERVER,
    LOCATION
};


class Block {
public:
    enum BlockType type;
    std::set<std::string> methods;
    std::map<std::string, std::string> contentTypes;
    std::string root;
    std::string errorsRoot;
    std::string index;
    size_t maxBodySize;

    Block(enum BlockType type);
    virtual ~Block() {}
};

class Location: public Block {
public:
    std::string endpoint;
    std::map<std::string, std::vector<std::string> > cgi;

    Location();
};

class Server: public Block {
public:
    std::vector<int> port;
    std::vector<int> serverFd;
    std::vector<struct sockaddr_in> addr;
    std::vector<Location> locations;

    Server();
    ~Server();

    void init();
    void addChild(Location &location);
    void addListen(size_t port);
};

class Config: public Block {
public:
    std::vector<Server> servers;
    std::map<int, Client> clients;
    std::map<int, int> cgiToClient;

    Config();
    ~Config();

    void addChild(Server &server);
    int init();
    void run();
    int isServerFd(int fd);
    void handleNewConnections(int serverFd, int serverIndex);
    void removeClient(int fd);
    void registerCgiPipe(int pipeFd, int clientFd);
    void checkTimeouts();
    void updateEpollEvent(int fd, uint32_t events);

private:
    int epollFd;
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
};

#endif
