#include "Block.hpp"

// BASE CLASS
Block::Block(enum BlockType BlockType)
: type(BlockType), root("/") {}

// LOCATION 
LocationBlock::LocationBlock()
: Block(LOCATION), endpoint("/") {}

void LocationBlock::addChild(Block &block) {
    (void)block;
}

void LocationBlock::addListen(size_t port) {
    (void)port;
}

// SERVER
ServerBlock::ServerBlock()
: Block(SERVER), listen(8080) {}

void ServerBlock::addChild(Block &block) {
    locations.push_back(static_cast<LocationBlock&>(block));
}

void ServerBlock::addListen(size_t port) {
    this->listen = port;
}

// MAIN
MainBlock::MainBlock()
: Block(MAIN) {}

void MainBlock::addChild(Block &block) {
    server = static_cast<ServerBlock&>(block);
}

void MainBlock::addListen(size_t port) {
    (void)port;
}
