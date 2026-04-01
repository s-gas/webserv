#include <fstream>
#include <iostream>
#include "Server.hpp"
#include "parseConf.hpp"

Server::Server(std::string fileName) 
:listen(80), root("/") {
    checkExtension(fileName);
    std::ifstream confFile(fileName.c_str());
    if (!confFile.is_open()) {
        throw std::runtime_error("Error: Unable to open " + fileName);
    }
    std::string line;
    bool isServerPresent[2];
    while (std::getline(confFile, line)) {
        checkForServerBlock(isServerPresent, line);
    }
    if (isServerPresent[0] && isServerPresent[1]) {
        std::cout << "server block present!" << std::endl;
    } else {
        std::cout << "server block not present!" << std::endl;
    }
    std::cout << "listen: " << listen << std::endl;
    std::cout << "root: " << root << std::endl;
}