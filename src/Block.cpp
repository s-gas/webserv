#include "Server.hpp"

Block::Block(enum BlockType BlockType)
: type(BlockType), root("www"), errorsRoot("www/errors"), index("index.html") {
    methods.insert("GET");
    methods.insert("POST");
    methods.insert("DELETE");

    contentTypes[".html"] = "text/html";
    contentTypes[".css"] = "text/html";
}
