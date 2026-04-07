#ifndef BLOCK_HPP
# define BLOCK_HPP

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

class LocationBlock: public Block {
public:
    std::string endpoint;

    LocationBlock();

    void addChild(Block &block);
    void addListen(size_t port);
};

class ServerBlock: public Block {
public:
    size_t listen;
    std::vector<LocationBlock> locations; 

    ServerBlock();

    void addChild(Block &block);
    void addListen(size_t port);
};

class MainBlock: public Block {
public:
    ServerBlock server;

    MainBlock();

    void addChild(Block &block);
    void addListen(size_t port);
};

#endif