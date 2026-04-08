#include "webserv.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <config file>" << std::endl;
    return 1;
  }
  std::ifstream configFile;
  std::string fileName = argv[1];
  Main main;
  try {
    checkExtension(fileName);
    readFile(configFile, fileName);
    parse(main, configFile);
    main.server.init();
    main.server.run();
  } catch (const std::exception &e) {
    LOG_ERROR << e.what();
    return 1;
  }
  return 0;
}
