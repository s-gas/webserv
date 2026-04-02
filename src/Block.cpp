#include "Block.hpp"

// BASE CLASS
Block::Block(enum BlockType BlockType)
: type(BlockType), root("/") {}

// LOCATION 
LocationBlock::LocationBlock()
: Block(LOCATION), endpoint("/") {}

void LocationBlock::add(Block &block) {
    (void)block;
}

// SERVER
ServerBlock::ServerBlock()
: Block(SERVER), listen(8080) {}

void ServerBlock::add(Block &block) {
    locations.push_back(static_cast<LocationBlock&>(block));
}

// MAIN
MainBlock::MainBlock()
: Block(MAIN) {}

void MainBlock::add(Block &block) {
    server = static_cast<ServerBlock&>(block);
}
