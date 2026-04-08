#ifndef SERVER_HPP
# define SERVER_HPP

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>

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

    virtual void addChild(Block &block) = 0;
    virtual void addListen(size_t port) = 0;
};

class Location: public Block {
public:
    std::string endpoint;

    Location();

    void addChild(Block &block);
    void addListen(size_t port);
};

class Server: public Block {
public:
    int _port;
    int _serverFd;
    struct sockaddr_in _address;
    std::vector<Location> locations;

    Server();
    ~Server();

    void init();
    void run();
    void addChild(Block &block);
    void addListen(size_t port);
};

class Main: public Block {
public:
    Server server;

    Main();

    void addChild(Block &block);
    void addListen(size_t port);
};

#endif