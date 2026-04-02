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
    virtual void add(Block &block) = 0;
};

class LocationBlock: public Block {
public:
    std::string endpoint;
    LocationBlock();
    void add(Block &block);
};

class ServerBlock: public Block {
public:
    size_t listen;
    std::vector<LocationBlock> locations; 
    ServerBlock();
    void add(Block &block);
};

class MainBlock: public Block {
public:
    ServerBlock server;
    MainBlock();
    void add(Block &block);
};

#endif