#include "Server.hpp"

Block::Block(enum BlockType BlockType)
: type(BlockType), root("www"), errorsRoot("www/errors"), index("index.html"), maxBodySize(0) {
    methods.insert("GET");
    methods.insert("POST");
    methods.insert("DELETE");

    contentTypes[".html"] = "text/html";
    contentTypes[".txt"] = "text/plain";
}
