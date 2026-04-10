#include "webserv.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <config file>" << std::endl;
    return 1;
  }
  std::ifstream configFile;
  std::string fileName = argv[1];
  Config main;
  try {
    checkExtension(fileName);
    readFile(configFile, fileName);
    parseConfig(main, configFile);
    main.server.init();
    main.run();
  } catch (const std::exception &e) {
    LOG_ERROR << e.what();
    return FAILURE;
  }
  return SUCCESS;
}
