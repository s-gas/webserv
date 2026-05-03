#include "Server.hpp"

Block::Block() {}

Block::Block(enum BlockType BlockType)
: type(BlockType), root("/") {}
