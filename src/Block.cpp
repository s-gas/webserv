#include "Server.hpp"

Block::Block(enum BlockType BlockType)
: type(BlockType), root("www/") {
    allowedMethods.insert("GET");
    allowedMethods.insert("POST");
    allowedMethods.insert("DELETE");

    contentTypes[".html"] = "text/html";
    contentTypes[".css"] = "text/html";
}
