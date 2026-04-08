#include "Server.hpp"

Main::Main()
: Block(MAIN) {}

void Main::addChild(Block &block) {
    server = static_cast<Server&>(block);
}

void Main::addListen(size_t port) {
    (void)port;
}