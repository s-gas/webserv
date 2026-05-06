#include "Server.hpp"

Block::Block(enum BlockType BlockType)
: type(BlockType), root("/") {
    allowedMethods.insert("GET");
    allowedMethods.insert("POST");
    allowedMethods.insert("DELETE");
}
