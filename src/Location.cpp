#include "Server.hpp"

Location::Location()
: Block(LOCATION), endpoint("/") {}

void Location::addChild(Block &block) {
    (void)block;
}

void Location::addListen(size_t port) {
    (void)port;
}