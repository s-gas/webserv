#include <fstream>
#include "Server.hpp"
#include "parseConf.hpp"

Server::Server(std::string fileName) {
    checkExtension(fileName);
    std::ifstream confFile(fileName.c_str());
    if (!confFile.is_open()) {
        throw std::runtime_error("Error: Unable to open " + fileName);
    }
    listen = 0;
    root = "root";
}