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
    std::string root;

    Block(enum BlockType type);
    virtual ~Block() {}
};

class Location: public Block {
public:
    std::string endpoint;

    Location();
};

class Server: public Block {
public:
    std::vector<int> _port;
    std::vector<int> _serverFd;
    std::vector<struct sockaddr_in> _addr;
    std::vector<Location> locations;
    std::set<std::string> allowedMethods;

    Server();
    ~Server();

    void init();
    void addChild(Location &location);
    void addListen(size_t port);
};

class Config: public Block {
public:
    std::vector<Server> _servers;
    std::set<std::string> allowedMethods;
    std::map<int, Client> clients;

    Config();
    ~Config();

    void addChild(Server &server);
    int init();
    void run();
    int isServerFd(int fd);
    void handleNewConnections(int serverFd, int serverIndex);
    void removeClient(int fd);

private:
    int _epollFd;
    struct epoll_event _event;
    struct epoll_event _events[MAX_EVENTS];
};

#endif
