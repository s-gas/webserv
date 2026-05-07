#include "Server.hpp"

Block::Block(enum BlockType BlockType)
: type(BlockType), root("www"), errorsRoot("www/errors"), index("index.html") {
    allowedMethods.insert("GET");
    allowedMethods.insert("POST");
    allowedMethods.insert("DELETE");

    contentTypes[".html"] = "text/html";
    contentTypes[".css"] = "text/html";
}
